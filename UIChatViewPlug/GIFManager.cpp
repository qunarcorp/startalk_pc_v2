//
// Created by QITMAC000260 on 2019-07-12.
//

#include "GIFManager.h"
#include <algorithm>

GIFManager::GIFManager() {
}

QMovie *GIFManager::getMovie(const QString &filePath) {
//    static std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    if(_mapMovie.contains(filePath))
    {
        auto* mov = _mapMovie[filePath];
        _movieCount[mov]++;
        return mov;
    }
    else
    {
        QMovie* mov = new QMovie(filePath);
        _mapMovie[filePath] = mov;
        _movieCount[mov] = 1;
        mov->setSpeed(80);
        return mov;
    }
}

void GIFManager::removeMovie(QMovie *mov) {
//    static std::lock_guard<QTalk::util::spin_mutex> lock(sm);

    if(_movieCount.contains(mov) && _movieCount[mov] >= 1)
    {
        _movieCount[mov]--;
        //
        if(_movieCount[mov] == 0)
        {
            _movieCount.remove(mov);
            //
            auto itFind = std::find_if(_mapMovie.begin(), _mapMovie.end(), [mov](QMovie* m){
                return m == mov;
            });
            if(itFind != _mapMovie.end())
                _mapMovie.remove(itFind.key());
            //
            delete mov;
            mov = nullptr;
        }
    }
}
