//
// Created by QITMAC000260 on 2018-12-25.
//

#include "NavManager.h"
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QUrlQuery>
#include "../CustomUi/QtMessageBox.h"
#include "LoginPanel.h"
#include "../Platform/Platform.h"

#define DEM_NAV_KEYS "navKeys"
#define DEM_DEFAULT_KEY "defaultKey"
#ifdef _QCHAT
#define DEM_DEFAULT_NAV ""
#else
#define DEM_DEFAULT_NAV ""
#endif

#define DEM_ROOT_TAG "NavConfig"

NavManager::NavManager(LoginPanel *loginPanel)
    : UShadowDialog(loginPanel, true)
    , _pLoginPanel(loginPanel)
{
    initConfig();
    initUi();
}

NavManager::~NavManager()
{

}

void NavManager::initConfig()
{
    std::string configDirPath = Platform::instance().getConfigPath();
    std::string oldConfig = configDirPath + "/NavConf";
    std::string newConfig = configDirPath + "/NavConf.data";

	QFileInfo oldConfigInfo(oldConfig.data());
	bool initoldconfig = false;
    if (oldConfigInfo.exists() && (oldConfigInfo.size() > 0) && !QFile::exists(newConfig.data()))
    {
        auto* _pConfigloader = new QTalk::ConfigLoader(oldConfig.c_str());
        if(_pConfigloader->reload())
        {
			initoldconfig = true;
            // 读取配置文件
            QString keys = QString::fromStdString(_pConfigloader->getString(DEM_NAV_KEYS));
            // 读取配置
            _defaultKey = QString::fromStdString(_pConfigloader->getString(DEM_DEFAULT_KEY));
            QStringList keyList = keys.split(",");

            for(const QString& key : keyList)
            {
                if(!_pConfigloader->hasKey(key.toStdString()))
                {
                    continue;
                }

                std::string val = _pConfigloader->getString(key.toStdString());
                QUrl url(val.data());

                QUrlQuery query(url);

                StNav stNav;
                stNav.name = query.queryItemValue("n");
                stNav.domain = query.queryItemValue("d");
                stNav.debug = query.queryItemValue("debug") == "true";
                query.removeQueryItem("n");
                query.removeQueryItem("d");
                query.removeQueryItem("debug");

                QString str = query.toString();
                stNav.url = url.toString().section("?", 0, 0);
                if (!str.isEmpty())
                    stNav.url += "?" + str;

                _mapNav[stNav.name] = stNav;
            }
            // 保存旧配置
            saveConfig();
            // 删除旧配置文件
            QFile::remove(oldConfig.data());
        }
        else
        {
            delete _pConfigloader;
        }
    }
	//
    if(!initoldconfig)
    {

        auto* navConfig = new QTalk::StConfig;

        if(!QFile::exists(configDirPath.data()))
        {
            QDir dir;
            dir.mkpath(configDirPath.data());
        }
        //
        if(QFile::exists(newConfig.data()))
        {
            QTalk::qConfig::loadConfig(newConfig.data(), false, navConfig);
            if(navConfig->tagName == DEM_ROOT_TAG &&
                navConfig->hasAttribute(DEM_DEFAULT_KEY) &&
                !navConfig->children.empty())
            {
                _defaultKey = navConfig->attribute(DEM_DEFAULT_KEY);

                for(const auto& item : navConfig->children)
                {
                    StNav stNav;
                    stNav.name = item->attribute("name");
                    stNav.domain = item->attribute("domain");
                    stNav.debug = item->attribute("debug") == "true";
                    stNav.url = item->attribute("url");

                    _mapNav[stNav.name] = stNav;
                }
                delete navConfig;
                return;
            }
            delete navConfig;
        }
//        else
        {
#ifndef _STARTALK
        // 默认设置
        StNav stNav;
#ifdef _QCHAT
        stNav.name = "QChat";
#else
        stNav.name = "QTalk";
#endif
        stNav.domain = _pLoginPanel->getDomainByNav(DEM_DEFAULT_NAV);
        stNav.url = DEM_DEFAULT_NAV;
        stNav.debug = false;

        _mapNav[stNav.name] = stNav;
        _defaultKey = stNav.name;

        saveConfig();
#endif
        }
    }

}

void NavManager::initUi()
{
    setFixedSize(630, 500);
    // top
    auto* topFrm = new QFrame(this);
    topFrm->setObjectName("NavManager_TopFrm");
    topFrm->setFixedHeight(50);
    auto * topLay = new QHBoxLayout(topFrm);
    auto* titleLabel = new QLabel(tr("配置导航"));
    titleLabel->setObjectName("NavManager_TitleLabel");
    _pCloseBtn = new QPushButton();
    titleLabel->setAlignment(Qt::AlignCenter);
    topLay->addWidget(titleLabel);
    topLay->addWidget(_pCloseBtn);

#ifdef _MACOS
    _pCloseBtn->setFixedSize(12, 12);
    _pCloseBtn->setObjectName("gmCloseBtn");
    topLay->addWidget(_pCloseBtn);
    topLay->addWidget(titleLabel);
    topLay->setContentsMargins(15, 6, 0, 0);
#else
    _pCloseBtn->setFixedSize(30, 30);
    _pCloseBtn->setObjectName("gwCloseBtn");
    topLay->setMargin(0);
    topLay->addWidget(titleLabel);
    topLay->addWidget(_pCloseBtn);
#endif
    //
    this->setMoverAble(true, topFrm);
    // main left
    auto* mainFrame = new QFrame(this);
    mainFrame->setObjectName("NavManager_MainFrm");
    auto * mainLay = new QHBoxLayout(mainFrame);
    mainLay->setMargin(0);
    _pNavView = new NavView(_mapNav, _defaultKey, this);
    mainLay->addWidget(_pNavView);

    for (const StNav& nav  : _mapNav.values())
    {
        emit _pNavView->addItemSignal(nav);
    }

    // main
    auto * layout = new QVBoxLayout(_pCenternWgt);
    layout->addWidget(topFrm);
    layout->addWidget(mainFrame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setStretch(1, 1);
    // 关闭按钮
    connect(_pCloseBtn, &QPushButton::clicked, [this](bool){this->setVisible(false);});
    connect(_pNavView, &NavView::saveConfSignal, this, &NavManager::onSaveConf);
    connect(_pNavView, &NavView::addNavSinal, this, &NavManager::onAddNav);
    connect(_pNavView, &NavView::sgClose, [this](){
        this->setVisible(false);
    });

    //
    _pNavView->onItemClicked(_defaultKey);
}

/**
 * 写配置
 */
void NavManager::saveConfig()
{
    std::string newConfig = Platform::instance().getConfigPath() + "/NavConf.data";
    auto* navConfig = new QTalk::StConfig(DEM_ROOT_TAG);

    for(const StNav& nav : _mapNav.values())
    {
        if(nav.url.isEmpty() || nav.name.isEmpty() || nav.domain.isEmpty())
        {
            continue;
        }

        auto* item = new QTalk::StConfig("item");
        item->setAttribute("name", nav.name);
        item->setAttribute("domain", nav.domain);
        item->setAttribute("url", nav.url);
        QString debug = (nav.debug ? "true" : "false");
        item->setAttribute("debug", debug);

        navConfig->addChild(item);
    }

    if(!navConfig->children.empty())
    {
        navConfig->setAttribute(DEM_DEFAULT_KEY,
                                _defaultKey.isEmpty() ?  navConfig->children[0]->attribute("name") : _defaultKey);
        //
        QTalk::qConfig::saveConfig(newConfig.data(), false, navConfig);
    }
}

/**
 * 获取导航地址
 * @return
 */
QString NavManager::getDefaultNavUrl()
{
    if(_mapNav.contains(_defaultKey))
    {
        StNav nav = _mapNav.value(_defaultKey);
        return nav.url;
    }
    return QString();
}

/**
 *
 */
void NavManager::onSaveConf()
{
    saveConfig();
}

/**
 *
 * @return
 */
QString NavManager::getNavName()
{
    bool debug = _mapNav[_defaultKey].debug;
	QString ret = _mapNav[_defaultKey].domain;
    if(debug)
    {
        ret += "_debug";
    }
    return ret;
}

/**
 *
 * @param name
 * @param navAddr
 * @param isDebug
 */
void NavManager::onAddNav(const QString &name, const QString &navAddr, const bool &isDebug)
{
    const QString& addr = navAddr;
    QString domain = _pLoginPanel->getDomainByNav(addr);
    if(domain.isEmpty())
    {
        QtMessageBox::warning(this, tr("警告"), tr("无效的导航地址"));
    }
    else
    {
        StNav nav;
        nav.name = name;
        nav.url = navAddr;
        nav.domain = domain;
        nav.debug = isDebug;

        _mapNav[name] = nav;
        //
        emit _pNavView->addItemSignal(nav);
        //
        saveConfig();
    }
}

QString NavManager::getDefaultDomain()
{
    if(!_defaultKey.isEmpty() && _mapNav.contains(_defaultKey))
    {
        QString tmp = _mapNav[_defaultKey].domain;
        if(!tmp.isEmpty() && _mapNav[_defaultKey].debug)
            tmp += "_debug";
        return tmp;
    }
    return QString();
}