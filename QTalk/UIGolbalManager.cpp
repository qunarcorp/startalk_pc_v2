#include "UIGolbalManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/Log.h"
#include "../UICom/qconfig/qconfig.h"
#include "../UICom/StyleDefine.h"

// 下面路径可以根据ConfigDataDir定位目录然后拼接成文件位置
const QString UIGolbalManager::DEFAULT_PluginManagerPath = ":/QTalk/config/PluginManager.json";
const QString UIGolbalManager::DEFAULT_PluginPath = "./";

UIGolbalManager *UIGolbalManager::_pInstance = nullptr;

#define USER_FOLDER "USER"
#define FILE_FOLDER "FILE"
#define HISTORYU_FOLDER "HISTORY"
#define LOG_LEVEL "LOG_LEVEL"
#define THEME "THEME"
#define FONT "FONT"

UIGolbalManager::~UIGolbalManager() {
    if (_pluginManager) {
        delete _pluginManager;
        _pluginManager = nullptr;
    }
}

UIGolbalManager *UIGolbalManager::GetUIGolbalManager() {
    if (_pInstance == nullptr) {
        _pInstance = new UIGolbalManager;

    }
    return _pInstance;
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
QJsonDocument UIGolbalManager::LoadJsonConfig(const QString &path) {
    if (path.isEmpty()) {
        return QJsonDocument();
    }

    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonParseError err{};
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
        if (err.error == QJsonParseError::NoError) {
            return doc;
        } else {
            qWarning() << "load json fail" << path;
            return QJsonDocument();
        }
    } else {
        qWarning() << "open json flie fail" << path;
        return QJsonDocument();
    }
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::SaveJsonConfig(const QJsonDocument &doc, const QString &filepath, const QString &defaultpath) {
    if (doc.isEmpty() || doc == QJsonDocument(QJsonObject()))//如果是空的不写入，防止清空原有文档
    {
        return;
    }
    QString path;
    if (filepath.isEmpty()) {
        path = defaultpath;
    } else {
        path = filepath;
    }
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QByteArray str = doc.toJson(QJsonDocument::Indented);
        file.write(str);
    } else {
        qWarning() << "open json flie fail" << path;
        return;
    }
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::SaveUIGolbalManager() {
    QJsonDocument jsonDoc(ToJson());
    //SaveJsonConfig(jsonDoc, QString(), _ConfigDataDir + DEFAULT_UIGolbalManagerPath);
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::SavePluginManager() {
    if (_pluginManager) {
        QJsonDocument jsonDoc(_pluginManager->ToJson());
        SaveJsonConfig(jsonDoc, Get("ManagerFilePath").toString(), DEFAULT_PluginManagerPath);
    }
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
QObject *UIGolbalManager::GetPluginInstanceQt(const QString &key) const {
    qDebug() << "GetPluginInstanceQt" << key;
    if (_pluginManager) {
        return _pluginManager->GetPluginInstanceQt(key);
    }
    return nullptr;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.27
  */
std::shared_ptr<QMap<QString, QObject *> > UIGolbalManager::GetAllPluginInstanceQt() const {
    if (_pluginManager) {
        return _pluginManager->GetAllPluginInstanceQt();
    }
    return nullptr;
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::Init() {
    // init setting
    _ConfigDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    std::string userPath, fileSavePath, historyPath;
    int logLevel = QTalk::logger::LEVEL_INVALID;
    _pSystemConfig = new QTalk::ConfigLoader(_ConfigDataDir.toLocal8Bit() + "/sysconfig");
    if (_pSystemConfig->reload()) {
        userPath = _pSystemConfig->getString(USER_FOLDER);
        fileSavePath = _pSystemConfig->getString(FILE_FOLDER);
        historyPath = _pSystemConfig->getString(HISTORYU_FOLDER);
        logLevel = _pSystemConfig->getInteger(LOG_LEVEL);
        _theme = _pSystemConfig->getInteger(THEME);
        _font = _pSystemConfig->getString(FONT);
        if(logLevel == QTalk::logger::LEVEL_INVALID)
            _pSystemConfig->setInteger(LOG_LEVEL, QTalk::logger::LEVEL_WARING);
    }

    if(logLevel == QTalk::logger::LEVEL_INVALID)
        logLevel = QTalk::logger::LEVEL_WARING;
    QTalk::logger::setLevel(logLevel);
    //
    if(_theme == 0)
        _theme = 1;
    AppSetting::instance().setThemeMode(_theme);
    //
    AppSetting::instance().setFont(_font);

    AppSetting::instance().setLogLevel(logLevel);

    int channel = _pSystemConfig->getInteger("CHANNEL");
    if(0 == channel)
        channel = 1; // product
    AppSetting::instance().setTestchannel(channel);

    if (!userPath.empty())
        _ConfigDataDir = QString::fromStdString(userPath);
    if (fileSavePath.empty())
        fileSavePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();

    // 创建文件夹
    QDir dir(_ConfigDataDir);
    if (!dir.exists()) {
        bool isOK = dir.mkpath(_ConfigDataDir);//创建多级目录
        if (!isOK)
            error_log("init user folder error {0}", userPath);
    }
    dir = QDir(QString::fromStdString(fileSavePath));
    if (!dir.exists()) {
        bool isOK = dir.mkpath(QString::fromStdString(fileSavePath));//创建多级目录
        if (!isOK)
            error_log("init user folder error {0}", fileSavePath);
    }
    // 设置全局路径
    Platform::instance().setAppdataRoamingPath(_ConfigDataDir.toStdString());
    AppSetting::instance().setFileSaveDirectory(fileSavePath);
    //
    if (historyPath.empty())
        historyPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();
    Platform::instance().setHistoryDir(historyPath);
    // 保存最新配置
    saveSysConfig();
    //
    InitPluginManager();
    InitStyleManager();
    //
    initThemeConfig();
}

void UIGolbalManager::saveSysConfig() {
    std::string hispath = Platform::instance().getHistoryDir();
    std::string userpath = Platform::instance().getAppdataRoamingPath();
    std::string filepath = AppSetting::instance().getFileSaveDirectory();

    int theme = AppSetting::instance().getThemeMode();
    std::string font = AppSetting::instance().getFont();
    if (_pSystemConfig) {
        _pSystemConfig->setString(FILE_FOLDER, filepath);
        _pSystemConfig->setString(USER_FOLDER, userpath);
        _pSystemConfig->setString(HISTORYU_FOLDER, hispath);
        _pSystemConfig->setInteger("CHANNEL", AppSetting::instance().getTestchannel());
        _pSystemConfig->setInteger(LOG_LEVEL, AppSetting::instance().getLogLevel());
        _pSystemConfig->setInteger(THEME, AppSetting::instance().getThemeMode());
        _pSystemConfig->setString(FONT, font);
        _pSystemConfig->saveConfig();
    }

    if(_theme != theme || _font != font)
    {
        _theme = theme;
        _font = font;
        //
        setStyleSheetAll();
        initThemeConfig();
    }
}

/**
  * @功能描述 初始化插件管理器
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::InitPluginManager() {
    if (!_pluginManager) {
        _pluginManager = new PluginManager;
        qDebug() << "InitPluginManager " << DEFAULT_PluginManagerPath;
        QJsonDocument doc = LoadJsonConfig(DEFAULT_PluginManagerPath);
        if (doc.isObject()) {
            _pluginManager->FromJson(doc.object());
        } else {

        }
        QString pluginPath;
        pluginPath = DEFAULT_PluginPath;

        _pluginManager->setPluginPath(pluginPath);
        _pluginManager->LoadPluginAllQt();

        qDebug() << "InitPluginManager done.";
    }
}

/**
  * @功能描述 初始化样式文件管理器
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::InitStyleManager() {
    if (!_pstyleSheetManager) {
        _pstyleSheetManager = new StyleSheetManager;
    }
}

/**
  * @功能描述 设置所有样式
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::setStyleSheetAll() {
    if (_pstyleSheetManager) {
        _pstyleSheetManager->setStyleSheets(_theme, _font);
    }
}

/**
  * @功能描述 设置公共样式
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::setStylesForApp() {
    if (_pstyleSheetManager) {
        _pstyleSheetManager->setStylesForApp(_theme, _font);
    }
}

/**
  * @功能描述 设置相应插件样式
  * @参数
  * @date 2018.9.17
  */
void UIGolbalManager::setStyleSheetForPlugin(const QString& plgName) {
    if (_pstyleSheetManager) {
        _pstyleSheetManager->setStyleSheetForPlugin(plgName, _theme);
    }
}

UIGolbalManager::UIGolbalManager() {
    _pluginManager = nullptr;
    _pstyleSheetManager = nullptr;
    _pSystemConfig = nullptr;
    Init();
}

bool UIGolbalManager::UnloadPluginQt(const QString &key) {
    if (nullptr != _pluginManager)
        return _pluginManager->UnloadPluginQt(key);

    return false;
}

/**
 *
 */
void UIGolbalManager::initThemeConfig()
{
    QString configPath = QString(":/style/style%1/data.xml").arg(_theme);
    auto *config = new QTalk::StConfig;
    QTalk::qConfig::loadConfig(configPath, false, config);
    if("Style" == config->tagName)
    {
        for(const auto it : config->children)
        {
            QString tagName = it->tagName;
            QString value = it->tagVal;

            int r = 255, g = 255, b = 255, a = 255;

            QStringList rgba = value.split(",");
            if(rgba.size() >= 4)
            {
                r = rgba[0].trimmed().toInt();
                g = rgba[1].trimmed().toInt();
                b = rgba[2].trimmed().toInt();
                a = rgba[3].trimmed().toInt();
            }
            else
            {
                error_log("error color {0} {1}", tagName.toStdString(), value.toStdString());
                continue;
            }

            if("_nav_normal_color" == tagName)
                QTalk::StyleDefine::instance().setNavNormalColor(QColor(r, g, b, a));
            else if ("_nav_select_color" == tagName)
                QTalk::StyleDefine::instance().setNavSelectColor(QColor(r, g, b, a));
            else if ("_nav_top_color" == tagName)
                QTalk::StyleDefine::instance().setNavTopColor(QColor(r, g, b, a));
            else if ("_nav_tip_color" == tagName)
                QTalk::StyleDefine::instance().setNavTipColor(QColor(r, g, b, a));
            else if ("_nav_name_font_color" == tagName)
                QTalk::StyleDefine::instance().setNavNameFontColor(QColor(r, g, b, a));
            else if ("_nav_content_font_color" == tagName)
                QTalk::StyleDefine::instance().setNavContentFontColor(QColor(r, g, b, a));
            else if ("_nav_time_font_color" == tagName)
                QTalk::StyleDefine::instance().setNavTimeFontColor(QColor(r, g, b, a));
            else if ("_nav_name_font_select_color" == tagName)
                QTalk::StyleDefine::instance().setNavNameSelectFontColor(QColor(r, g, b, a));
            else if ("_nav_content_font_select_color" == tagName)
                QTalk::StyleDefine::instance().setNavContentSelectFontColor(QColor(r, g, b, a));
            else if ("_nav_time_font_select_color" == tagName)
                QTalk::StyleDefine::instance().setNavTimeSelectFontColor(QColor(r, g, b, a));
            else if ("_nav_at_font_color" == tagName)
                QTalk::StyleDefine::instance().setNavAtFontColor(QColor(r, g, b, a));
            else if ("_nav_unread_font_color" == tagName)
                QTalk::StyleDefine::instance().setNavUnReadFontColor(QColor(r, g, b, a));
            else if ("_chat_group_normal_color" == tagName)
                QTalk::StyleDefine::instance().setChatGroupNormalColor(QColor(r, g, b, a));
            else if ("_chat_group_font_color" == tagName)
                QTalk::StyleDefine::instance().setChatGroupFontColor(QColor(r, g, b, a));
            else if ("_adr_normal_color" == tagName)
                QTalk::StyleDefine::instance().setAdrNormalColor(QColor(r, g, b, a));
            else if ("_adr_select_color" == tagName)
                QTalk::StyleDefine::instance().setAdrSelectColor(QColor(r, g, b, a));
            else if ("_adr_name_font_color" == tagName)
                QTalk::StyleDefine::instance().setAdrNameFontColor(QColor(r, g, b, a));
            else if ("_drop_normal_color" == tagName)
                QTalk::StyleDefine::instance().setDropNormalColor(QColor(r, g, b, a));
            else if ("_drop_select_color" == tagName)
                QTalk::StyleDefine::instance().setDropSelectColor(QColor(r, g, b, a));
            else if ("_drop_normal_font_color" == tagName)
                QTalk::StyleDefine::instance().setDropNormalFontColor(QColor(r, g, b, a));
            else if ("_drop_select_font_color" == tagName)
                QTalk::StyleDefine::instance().setDropSelectFontColor(QColor(r, g, b, a));
            else if("_link_url_color" == tagName)
                QTalk::StyleDefine::instance().setLinkUrl(value);
            else if("_local_search_time_font_color" == tagName)
                QTalk::StyleDefine::instance().setLocalSearchTimeFontColor(QColor(r, g, b, a));
            else if("_at_block_font_color" == tagName)
                QTalk::StyleDefine::instance().setAtBlockFontColor(QColor(r, g, b, a));
            else if("_emo_select_icon_color" == tagName)
                QTalk::StyleDefine::instance().setEmoSelectIconColor(QColor(r, g, b, a));
            else if("_group_card_group_member_normal_color" == tagName)
                QTalk::StyleDefine::instance().setGroupCardGroupMemberNormalColor(QColor(r, g, b, a));
            else if("_group_manager_normal_color" == tagName)
                QTalk::StyleDefine::instance().setGroupManagerNormalColor(QColor(r, g, b, a));
            else if("_group_manager_normal_font_color" == tagName)
                QTalk::StyleDefine::instance().setGroupManagerNormalFontColor(QColor(r, g, b, a));
            else if("_title_search_normal_color" == tagName)
                QTalk::StyleDefine::instance().setTitleSearchNormalColor(QColor(r, g, b, a));
            else if("_title_search_select_color" == tagName)
                QTalk::StyleDefine::instance().setTitleSearchSelectColor(QColor(r, g, b, a));
            else if("_quote_block_background_color" == tagName)
                QTalk::StyleDefine::instance().setQuoteBlockBackgroundColor(QColor(r, g, b, a));
            else if("_quote_block_tip_color" == tagName)
                QTalk::StyleDefine::instance().setQuoteBlockTipColor(QColor(r, g, b, a));
            else if("_quote_block_font_color" == tagName)
                QTalk::StyleDefine::instance().setQuoteBlockFontColor(QColor(r, g, b, a));
            else if("_file_process_bar_background" == tagName)
                QTalk::StyleDefine::instance().setFileProcessBarBackground(QColor(r, g, b, a));
            else if("_file_process_bar_line" == tagName)
                QTalk::StyleDefine::instance().setFileProcessBarLine(QColor(r, g, b, a));
            else if("_head_photo_mask_color" == tagName)
                QTalk::StyleDefine::instance().setHeadPhotoMaskColor(QColor(r, g, b, a));
        }
    }
}
