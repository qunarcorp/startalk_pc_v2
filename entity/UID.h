//
// Created by QITMAC000260 on 2019-03-13.
//

#ifndef QTALK_V2_UID_H
#define QTALK_V2_UID_H

#include <sstream>
#include <string>


namespace QTalk {
    namespace Entity {

        class UID {

        public:
            explicit UID() = default;

            explicit UID(const std::string& usrId, const std::string& realId = std::string())
            {
                _usrId = usrId;

                if(realId.empty())
                {
                    auto index = usrId.find("<>");
                    if(index != std::string::npos)
                    {
                        _usrId = usrId.substr(0, index);
                        _realId = usrId.substr(index + 2);
                    }
                    else
                    {
                        _realId = usrId;
                    }
                }
                else
                    _realId = realId;
            }

#if defined(QT_VERSION)
            explicit UID(const QString& usrId, const QString& realId = QString())
            {
                _usrId = usrId.toStdString();
                if(realId.isEmpty())
                {
                    if(usrId.contains("<>"))
                    {
                        _usrId = usrId.section("<>", 0 , 0).toStdString();
                        _realId = usrId.section("<>", 1 , 1).toStdString();
                    }
                    else
                    {
                        _realId = usrId.toStdString();
                    }
                }
                else
                    _realId = realId.toStdString();;
            }
#endif

        public:
            bool isEmpty()
            {
                return _usrId.empty() && _realId.empty();
            }

            size_t size() const {
                return _usrId.size() + _realId.size();
            }

            bool isConsult()
            {
                return _usrId != _realId;
            }

            std::string usrId() const
            {
                return _usrId;
            }

            std::string realId() const
            {
                return _realId;
            }

public:
            std::string toStdString() const {
                std::ostringstream ss;
                ss << _usrId
                   << "<>"
                   << _realId;

                return ss.str();
            }

#if defined(QT_VERSION)
            QString toQString()
            {
                QString ret = QString("%1<>%2").arg(_usrId.data()).arg(_realId.data());
                return ret;
            }

            QString qUsrId() const
            {
                return QString::fromStdString(_usrId);
            }

            QString qReadJid() const
            {
                return QString::fromStdString(_realId);
            }
#endif

        public:
            bool operator==(const UID &other) const
            {
                if (other.size() != this->size())
                    return false;

                return this->_usrId == other._usrId && this->_realId == other._realId;;
            }

            bool operator<(const UID& other) const
            {
                return (this->toStdString() < other.toStdString());
            }


        private:
            std::string _usrId;
            std::string _realId;
        };

    }
}

#endif //QTALK_V2_UID_H
