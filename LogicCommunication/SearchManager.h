#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

class Communication;
class SearchInfoEvent;
class SearchManager
{
public:
    SearchManager(Communication* pComm);
    void GetSearchResult(SearchInfoEvent &e);

private:
    Communication* _pComm;
};

#endif // SEARCHMANAGER_H
