#include <mutex>


#include "NavigationManager.h"

NavigationManager* NavigationManager::_navManager = nullptr;

NavigationManager& NavigationManager::instance() {
    if (nullptr == _navManager)
    {
        static NavigationManager navManager;
        _navManager = &navManager;
    }
    return *_navManager;
}

NavigationManager::NavigationManager()
{
	rsaEncodeType = 0;
	probufPort = 0;
}

void NavigationManager::setVersion(QInt64 verion)
{
    navVersion = verion;
}

void NavigationManager::setXmppHost(const std::string &xmppHost)
{
    this->xmppHost = xmppHost;
}

std::string NavigationManager::getXmppHost() {
    return this->xmppHost;
}

void NavigationManager::setDomain(const std::string &domain)
{
    this->domain = domain;
}

std::string NavigationManager::getDomain() {
    return this->domain;
}

void NavigationManager::setProbufPort(int port)
{
    this->probufPort = port;
}

int NavigationManager::getProbufPort() {
    return this->probufPort;
}

void NavigationManager::setApiHost(const std::string &url)
{
    this->apiurl = url;
}

std::string NavigationManager::getApiUrl() {
    return this->apiurl;
}

void NavigationManager::setJavaHost(const std::string& javaUrl)
{
    this->javaUrl = javaUrl;
}

std::string NavigationManager::getJavaHost() {
    return this->javaUrl;
}

void NavigationManager::setHttpHost(const std::string &httpurl)
{
    this->httpurl = httpurl;
}

std::string NavigationManager::getHttpHost() {
    return this->httpurl;
}

void NavigationManager::setPubKey(const std::string &pubKey)
{
    this->pubkey = pubKey;
}

std::string NavigationManager::getPubkey() {
    return this->pubkey;
}

void NavigationManager::setFileHttpHost(const std::string &fileHttpHost)
{
    this->fileurl = fileHttpHost;
}

std::string NavigationManager::getFileHttpHost()
{
    return this->fileurl;
}

void NavigationManager::setPhoneNumAddr(const std::string &phone) {
    phoneAddr = phone;
}

std::string NavigationManager::getPhoneNumAddr() {
    return phoneAddr;
}

void NavigationManager::setLeaderUrl(const std::string &leaderUrl) {
    this->leaderUrl = leaderUrl;
}

std::string NavigationManager::getLeaderUrl() {
    return this->leaderUrl;
}

void NavigationManager::setRsaEncodeType(int t)
{
	rsaEncodeType = t;
}

int NavigationManager::getRsaEncodeType()
{
	return rsaEncodeType;
}

void NavigationManager::setUploadLog(const std::string& lodAddr)
{
    uploadLog = lodAddr;
}

std::string NavigationManager::getUploadLog() {
    return uploadLog;
}

void NavigationManager::setFoundConfigUrl(const std::string &url) {
    this->foundConfigUrl = url;
}

std::string NavigationManager::getFoundConfigUrl() {
    return foundConfigUrl;
}

void NavigationManager::setOpsApiHost(const std::string &opsHost)
{
    this->opsApiHost = opsHost;
}

std::string NavigationManager::getOpsApiHost() {
    return this->opsApiHost;
}

std::string NavigationManager::getQCHost() {
    return this->qcHost;
}

void NavigationManager::setQCHost(const std::string &qcHost)
{
    this->qcHost = qcHost;
}

std::string NavigationManager::getHealthCheckUrl() {
    return this->healthcheckUrl;
}

std::string NavigationManager::getQCGrabOrderUrl() {
    return this->qcGrabOrderUrl;
}

std::string NavigationManager::getQCOrderManager() {
    return this->qcOrderManager;
}

std::string NavigationManager::getAccountConfigUrl() {
    return this->accountConfigUrl;
}

bool NavigationManager::getReadFlag() {
    return this->readFlag;
}

std::string NavigationManager::getADUrl() {
    return this->adUrl;
}

int NavigationManager::getADSec() {
    return this->adSec;
}

void NavigationManager::setSearchUrl(const std::string &searchUrl) {
    this->searchUrl = searchUrl;
}

std::string NavigationManager::getSearchUrl() {
    return this->searchUrl;
}

void NavigationManager::setMailSuffix(const std::string &mailSuffix) {
    this->mailSuffix = mailSuffix;
}

std::string NavigationManager::getMailSuffix() {
    return this->mailSuffix;
}

void NavigationManager::setShareUrl(const std::string &shareUrl)
{
    this->shareUrl = shareUrl;
}

std::string NavigationManager::getShareUrl() {
    return shareUrl;
}

void NavigationManager::setQcadminHost(const std::string &adminHost) {
    this->qcadminHost = adminHost;
}

std::string NavigationManager::getQcadminHost() {
    return qcadminHost;
}

void NavigationManager::setShowmsgstat(const bool showmsgstat) {
    this->showmsgstat = showmsgstat;
}

bool NavigationManager::isShowmsgstat() {
    return showmsgstat;
}

void NavigationManager::setQcGrabOrder(const std::string qcGrabOrder) {
    this->qcGrabOrder = qcGrabOrder;
}

std::string NavigationManager::getQcGrabOrder() {
    return qcGrabOrder;
}

