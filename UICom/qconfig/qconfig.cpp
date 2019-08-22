//
// Created by cc on 19-1-13.
//

#include "qconfig.h"
#include <QDomDocument>
#include <QFile>

namespace QTalk {
    namespace qConfig {
        //
        void getXmlContent(const QDomElement &ele, StConfig *config) {
            config->tagName = ele.tagName();
            // attr
            QDomNamedNodeMap attrs = ele.attributes();
            for (int i = 0; i < attrs.size(); i++) {
                QDomNode node = attrs.item(i);
                if (!node.isNull()) {
                    QString nodeName = node.nodeName();
                    QString nodeVal = node.nodeValue();
                    QPair<QString, QString> attrPair(nodeName, nodeVal);
                    config->attributes.push_back(attrPair);
                }
            }
            //
            QDomNode tmpN = ele.firstChild();
            while (!tmpN.isNull()) {
                if (tmpN.nodeType() == QDomNode::TextNode) {
                    config->tagVal = tmpN.nodeValue();
                } else {
                    QDomElement temE = tmpN.toElement();
                    auto *tmpConfig = new StConfig;
                    tmpConfig->parent = config;
                    getXmlContent(temE, tmpConfig);
                    config->children.push_back(tmpConfig);
                }
                tmpN = tmpN.nextSibling();
            }
        }

        //
        void setXMlContent(QDomDocument &doc, QDomElement &ele, const StConfig *config) {
            if (nullptr == config)
                return;

            // attr
            for (QPair<QString, QString> attr : config->attributes) {
                ele.setAttribute(attr.first, attr.second);
            }
            //
            if (!config->tagVal.isEmpty()) {
                QDomText textNde = doc.createTextNode(config->tagVal);
                ele.appendChild(textNde);
            }
            // child
            for (const StConfig *tmpConf : config->children) {
                if (nullptr == tmpConf)
                    continue;
                //
                QDomElement tmpEle = doc.createElement(tmpConf->tagName);
                setXMlContent(doc, tmpEle, tmpConf);
                ele.appendChild(tmpEle);
            }
        }

        //
        bool loadConfig(const QString &xmlPath, bool encrypt, StConfig *config) {
            QFile file(xmlPath);
            if (!file.exists())
                return false;
            if (!file.open(QIODevice::ReadOnly))
                return false;
            //
            QByteArray xmlData = file.readAll();
            if (encrypt)
                xmlData = QByteArray::fromBase64(xmlData);
            //
            QDomDocument doc;
            if (doc.setContent(xmlData)) {
                QDomElement root = doc.documentElement();
                getXmlContent(root, config);
                //
                return true;
            }
            return false;
        }

        bool saveConfig(const QString &xmlPath, bool encrypt, const StConfig *config) {
            if (nullptr == config)
                return false;
            QFile xmlFile(xmlPath);
            if (!xmlFile.open(QFile::WriteOnly | QFile::Truncate))
                return false;
            //
            QDomDocument doc;
            QDomElement root = doc.createElement(config->tagName);
            setXMlContent(doc, root, config);
            doc.appendChild(root);
            //
            QByteArray xmlData = doc.toByteArray();
            if (encrypt) {
                xmlData = xmlData.toBase64();
            }
            xmlFile.resize(0);
            xmlFile.write(xmlData);
            xmlFile.close();
            //
            return true;
        }
    }
}