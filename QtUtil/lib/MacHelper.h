//
// Created by may on 2019-03-21.
//

#ifndef QTALK_V2_MACHELPER_H
#define QTALK_V2_MACHELPER_H

#include <string>

class MacHelper {

public:
    static std::string runCommand(const char *command, char **arguments = nullptr, int count = 0);

};


#endif //QTALK_V2_MACHELPER_H
