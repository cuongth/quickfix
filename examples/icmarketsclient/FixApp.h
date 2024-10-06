#ifndef FIXAPP_H
#define FIXAPP_H

#include <iostream>
#include <vector>
#include "quickfix/Application.h"
#include "quickfix/FileLog.h"
#include "quickfix/FileStore.h"
#include "quickfix/fix44/CollateralInquiry.h"
#include "quickfix/fix44/CollateralInquiryAck.h"
#include "quickfix/fix44/CollateralReport.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataRequestReject.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/NewOrderList.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/PositionReport.h"
#include "quickfix/fix44/RequestForPositions.h"
#include "quickfix/fix44/RequestForPositionsAck.h"
#include "quickfix/fix44/SecurityList.h"
#include "quickfix/fix44/TradingSessionStatus.h"
#include "quickfix/fix44/TradingSessionStatusRequest.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/SessionID.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/SocketInitiator.h"

using namespace std;
using namespace FIX;

class FixApp: public MessageCracker, public Application
{
private:
  SessionSettings  *settings;
  FileStoreFactory *store_factory;
  FileLogFactory   *log_factory;
  SocketInitiator  *initiator;

  unsigned int requestID;
  SessionID sessionID(bool md);
  vector<SessionID> sessions;
  vector<string> list_accountID;

  // Custom FXCM FIX fields
  enum FXCM_FIX_FIELDS
  {
    FXCM_FIELD_PRODUCT_ID      = 9080,
    FXCM_POS_ID                = 9041,
    FXCM_POS_OPEN_TIME         = 9042,
    FXCM_ERROR_DETAILS         = 9029,
    FXCM_REQUEST_REJECT_REASON = 9025,
    FXCM_USED_MARGIN           = 9038,
    FXCM_POS_CLOSE_TIME        = 9044,
    FXCM_MARGIN_CALL           = 9045,
    FXCM_ORD_TYPE              = 9050,
    FXCM_ORD_STATUS            = 9051,
    FXCM_CLOSE_PNL             = 9052,
    FXCM_SYM_POINT_SIZE        = 9002,
    FXCM_SYM_PRECISION         = 9001,
    FXCM_TRADING_STATUS        = 9096,
    FXCM_PEG_FLUCTUATE_PTS     = 9061,
    FXCM_NO_PARAMS             = 9016,
    FXCM_PARAM_NAME            = 9017,
    FXCM_PARAM_VALUE           = 9018
  };

  void queryHeader( FIX::Header& header );
  SenderCompID querySenderCompID();
  TargetCompID queryTargetCompID();
  TargetSubID queryTargetSubID();
  bool queryConfirm( const string& query );
  FIX44::MarketDataRequest queryMarketDataRequest44();

public:
  FixApp();
  void onCreate(const SessionID& session_ID);
  void onLogon(const SessionID& session_ID);
  void onLogout(const SessionID& session_ID);
  void toAdmin(Message& message, const SessionID& session_ID);
  void toApp(Message& message, const SessionID& session_ID);
  void fromAdmin(const Message& message, const SessionID& session_ID);
  void fromApp(const Message& message, const SessionID& session_ID);

  void onMessage(const FIX44::TradingSessionStatus& tss, const SessionID& session_ID);
  void onMessage(const FIX44::CollateralInquiryAck& ack, const SessionID& session_ID);
  void onMessage(const FIX44::CollateralReport& cr, const SessionID& session_ID);
  void onMessage(const FIX44::RequestForPositionsAck& ack, const SessionID& session_ID);
  void onMessage(const FIX44::PositionReport& pr, const SessionID& session_ID);
  void onMessage(const FIX44::MarketDataRequestReject& mdr, const SessionID& session_ID);
  void onMessage(const FIX44::MarketDataSnapshotFullRefresh& mds, const SessionID& session_ID);
  void onMessage(const FIX44::ExecutionReport& er, const SessionID& session_ID);

  void StartSession(string &);
  void EndSession();

  void GetTradingStatus();
  void GetAccounts();
  void GetPositions();
  void SubscribeMarketData();
  void UnsubscribeMarketData();
  void MarketOrder();
  string NextRequestID();
  void RecordAccount(string accountID);
  void queryMarketDataRequest();

};

#endif // FIXAPP_H
