//
// Created by QITMAC000260 on 2019-02-28.
//

#include "CodeShell.h"
#include <QToolButton>
#include <QHBoxLayout>

CodeShell::CodeShell(const QString& title, QWidget* contentWgt, QWidget* parent)
    : QFrame(parent), _title(title), _pContentWgt(contentWgt)
{
    initUi();
}

CodeShell::~CodeShell()
{

}

/**
 *
 */
void CodeShell::initUi()
{
    setMinimumSize(650, 520);
    setObjectName("CodeShell");
    // title
    _pTitleLabel = new QLabel(_title, this);
    _pTitleLabel->setObjectName("TitleLabel");
    _pTitleLabel->setAlignment(Qt::AlignCenter);
    auto* closeBtn = new QToolButton(this);
    _pTitleFrm = new QFrame(this);
    _pTitleFrm->setFixedHeight(50);
    _pTitleFrm->setObjectName("titleFrm");
    auto* titleLay = new QHBoxLayout(_pTitleFrm);
#ifdef _MACOS
    titleLay->addWidget(closeBtn);
    titleLay->addWidget(_pTitleLabel);
    titleLay->setContentsMargins(10, 0, 20, 0);
    closeBtn->setObjectName("gmCloseBtn");
    closeBtn->setFixedSize(12, 12);
#else
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(_pTitleLabel);
    titleLay->addWidget(closeBtn);
    closeBtn->setFixedSize(20, 20);
    titleLay->setContentsMargins(30, 0, 10, 0);
#endif
    // tool
    _pCodeStyle = new QComboBox(this);
    _pCodeLanguage = new QComboBox(this);
    _pCodeStyle->setObjectName("CodeStyle");
    _pCodeLanguage->setObjectName("CodeLanguage");

    auto* toolFrm = new QFrame(this);
    auto* toolLay = new QHBoxLayout(toolFrm);

    toolLay->setMargin(0);
    toolLay->setSpacing(20);
    toolLay->addWidget(new QLabel(tr("代码风格"), this), 0);
    toolLay->addWidget(_pCodeStyle);
    toolLay->addItem(new QSpacerItem(40, 10, QSizePolicy::Fixed));
    toolLay->addWidget(new QLabel(tr("编程语言"), this), 0);
    toolLay->addWidget(_pCodeLanguage);
    toolLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    // main
    QFrame* bodyFrm = new QFrame(this);
    bodyFrm->setObjectName("bodyFrm");
    auto* bodyLay = new QVBoxLayout(bodyFrm);
    bodyLay->setSpacing(20);
    bodyLay->setContentsMargins(30, 20, 30, 20);
    bodyLay->addWidget(toolFrm, 0);
    bodyLay->addWidget(_pContentWgt, 1);

    _pMainLay = new QVBoxLayout(this);
    _pMainLay->setMargin(0);
    _pMainLay->setSpacing(0);
    _pMainLay->addWidget(_pTitleFrm, 0);
    _pMainLay->addWidget(bodyFrm, 1);

    QStringList styles;
    styles << "a11y-dark"
           << "a11y-light"
           << "agate"
           << "an-old-hope"
           << "androidstudio"
           << "arduino-light"
           << "arta"
           << "ascetic"
           << "atom-one-dark"
           << "atom-one-light"
           << "brown-paper"
           << "codepen-embed"
           << "color-brewer"
           << "darcula"
           << "dark"
           << "darkula"
           << "default"
           << "docco"
           << "dracula"
           << "far"
           << "foundation"
           << "github-gist"
           << "github"
           << "gml"
           << "googlecode"
           << "grayscale"
           << "gruvbox-dark"
           << "gruvbox-light"
           << "hopscotch"
           << "hybrid"
           << "idea"
           << "ir-black"
           << "isbl-editor-dark"
           << "isbl-editor-light"
           << "kimbie.dark"
           << "kimbie.light"
           << "lightfair"
           << "magula"
           << "mono-blue"
           << "monokai-sublime"
           << "monokai"
           << "nord"
           << "obsidian"
           << "ocean"
           << "paraiso-dark"
           << "paraiso-light"
           << "pojoaque"
           << "purebasic"
           << "qtcreator_dark"
           << "qtcreator_light"
           << "railscasts"
           << "rainbow"
           << "routeros"
           << "school-book"
           << "solarized-dark"
           << "solarized-light"
           << "sunburst"
           << "tomorrow-night"
           << "tomorrow"
           << "vs"
           << "vs2015"
           << "xcode"
           << "xt256"
           << "zenburn";

    QStringList languages;
    languages
            << "plaintext"
            << "json"
            << "c"
            << "c++"
            << "c#"
            << "objectivec"
            << "xml"
            << "java"
            << "sql"
            << "php"
            << "javascript"
            << "dos"
            << "ruby"
            << "ini"
            << "vbscript"
            << "css"
            << "lua"
            << "python"
            << "shell"
            << "perl"
            << "markdown"
            << "vim"
            << "properties"
            << "makefile";
    _pCodeStyle->addItems(styles);
    _pCodeLanguage->addItems(languages);

    connect(closeBtn, &QToolButton::clicked, [this](){emit closeWnd();});

    auto changeFun = [this](const QString&){
        QString style = _pCodeStyle->currentText();
        QString language = _pCodeLanguage->currentText();

        emit styleChanged(style, language);
    };

    connect(_pCodeStyle, &QComboBox::currentTextChanged, changeFun);
    connect(_pCodeLanguage, &QComboBox::currentTextChanged, changeFun);
}

QString CodeShell::getCodeStyle()
{
    return _pCodeStyle->currentText();
}

QString CodeShell::getCodeLanguage()
{
    return _pCodeLanguage->currentText();
}

bool CodeShell::setCodeStyle(const QString &style) {

    int styleIndex = _pCodeStyle->findText(style);
    if(styleIndex != -1)
        _pCodeStyle->setCurrentIndex(styleIndex);
    else
        _pCodeStyle->setCurrentIndex(0);

    return styleIndex != -1;

}

bool CodeShell::setCodeLanguage(const QString &language) {

    int styleIndex = _pCodeLanguage->findText(language);
    if(styleIndex != -1)
        _pCodeLanguage->setCurrentIndex(styleIndex);
    else
        _pCodeLanguage->setCurrentIndex(0);

    return styleIndex != -1;

}