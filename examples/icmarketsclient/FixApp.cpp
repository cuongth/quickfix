#include "FixApp.h"

// Returns SessionID - MarketData for md = true, Trading fr md = false
SessionID FixApp::sessionID(bool md)
{
  for (auto session : sessions)
  {
    // for FXCM MarketData sessions begin with MD_
    if ((session.toString().find("MD_") != string::npos) == md)
      return session;
  }
  return SessionID();
}

FixApp::FixApp()
{
  requestID = 1;
}

void FixApp::onCreate(const SessionID& session_ID)
{
  cout << "Session -> created" << endl;
  sessions.push_back(session_ID);
}

void FixApp::onLogon(const SessionID& session_ID)
{
  cout << "Session -> logon" << endl;
  GetTradingStatus();
}

void FixApp::onLogout(const SessionID& session_ID)
{
  // Session logout
  cout << "Session -> logout" << endl;
}

void FixApp::toAdmin(Message& message, const SessionID& session_ID)
{
  string msg_type = message.getHeader().getField(FIELD::MsgType);
  if(msg_type == "A")
  {
    string user = settings->get().getString("Username");
    string pass = settings->get().getString("Password");
    message.setField(Username(user));
    message.setField(Password(pass));
  }
  string sub_ID = settings->get(session_ID).getString("TargetSubID");
  message.getHeader().setField(TargetSubID(sub_ID));
}

void FixApp::toApp(Message& message, const SessionID& session_ID)
{
  string sub_ID = settings->get(session_ID).getString("TargetSubID");
  message.getHeader().setField(TargetSubID(sub_ID));
  cout << "[cuongth] set TargetSubID=" << sub_ID << endl;
}

void FixApp::fromAdmin(const Message& message, const SessionID& session_ID)
{
  crack(message, session_ID);
}

void FixApp::fromApp(const Message& message, const SessionID& session_ID)
{
  crack(message, session_ID);
}

void FixApp::onMessage(const FIX44::TradingSessionStatus& tss, const SessionID& session_ID)
{
  string trad_status = tss.getField(FIELD::TradSesStatus);
  cout << "TradingSessionStatus -> TradSesStatus -" << trad_status << endl;
  cout << "  SecurityList via TradingSessionStatus -> " << endl;
  int symbols_count = IntConvertor::convert(tss.getField(FIELD::NoRelatedSym));
  for(int i = 1; i <= symbols_count; i++)
  {
    FIX44::SecurityList::NoRelatedSym symbols_group;
    tss.getGroup(i,symbols_group);
    string symbol = symbols_group.getField(FIELD::Symbol);
    cout << "    Symbol -> " << symbol << endl;
  }
  cout << "  System Parameters via TradingSessionStatus -> " << endl;
  int params_count = IntConvertor::convert(tss.getField(FXCM_NO_PARAMS));
  for(int i = 1; i < params_count; i++)
  {
    FIX::FieldMap field_map = tss.getGroupRef(i,FXCM_NO_PARAMS);
    cout << "    Param Name -> " << field_map.getField(FXCM_PARAM_NAME)
    	 << " - Param Value -> " << field_map.getField(FXCM_PARAM_VALUE)
	 << endl;
  }
  GetAccounts();

}

void FixApp::onMessage(const FIX44::CollateralInquiryAck& ack, const SessionID& session_ID)
{}

void FixApp::onMessage(const FIX44::CollateralReport& cr, const SessionID& session_ID)
{
  cout << "CollateralReport -> " << endl;
  string accountID = cr.getField(FIELD::Account);
  string balance = cr.getField(FIELD::CashOutstanding);
  cout << "  AccountID -> " << accountID << endl;
  cout << "  Balance -> " << balance << endl;
  FIX44::CollateralReport::NoPartyIDs group;
  cr.getGroup(1,group); // CollateralReport will only have 1 NoPartyIDs group
  cout << "  Parties -> "<< endl;
  int number_subID = IntConvertor::convert(group.getField(FIELD::NoPartySubIDs));
  for(int u = 1; u <= number_subID; u++)
  {
    FIX44::CollateralReport::NoPartyIDs::NoPartySubIDs sub_group;
    group.getGroup(u,sub_group);
    string sub_type = sub_group.getField(FIELD::PartySubIDType);
    string sub_value = sub_group.getField(FIELD::PartySubID);
    cout << "    " << sub_type << " -> " << sub_value << endl;
  }
  RecordAccount(accountID);
}

void FixApp::onMessage(const FIX44::RequestForPositionsAck& ack, const SessionID& session_ID)
{
  string pos_reqID = ack.getField(FIELD::PosReqID);
  cout << "RequestForPositionsAck -> PosReqID - " << pos_reqID << endl;
  if(ack.isSetField(FIELD::Text))
    cout << "RequestForPositionsAck -> Text - " << ack.getField(FIELD::Text) << endl;
}

void FixApp::onMessage(const FIX44::PositionReport& pr, const SessionID& session_ID)
{
  string accountID = pr.getField(FIELD::Account);
  string symbol = pr.getField(FIELD::Symbol);
  string positionID = pr.getField(FXCM_POS_ID);
  string pos_open_time = pr.getField(FXCM_POS_OPEN_TIME);
  cout << "PositionReport -> " << endl;
  cout << "   Account -> " << accountID << endl;
  cout << "   Symbol -> " << symbol << endl;
  cout << "   PositionID -> " << positionID << endl;
  cout << "   Open Time -> " << pos_open_time << endl;
}

void FixApp::onMessage(const FIX44::MarketDataRequestReject& mdr, const SessionID& session_ID)
{
  cout << "MarketDataRequestReject -> " << endl;
  if(mdr.isSetField(FIELD::Text))
    cout << " Text -> " << mdr.getField(FIELD::Text) << endl;
}

void FixApp::onMessage(const FIX44::MarketDataSnapshotFullRefresh& mds, const SessionID& session_ID)
{
  string symbol = mds.getField(FIELD::Symbol);
  double bid_price = 0;
  double ask_price = 0;
  int entry_count = IntConvertor::convert(mds.getField(FIELD::NoMDEntries));
  for(int i = 1; i < entry_count; i++)
  {
    FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;
    mds.getGroup(i,group);
    string entry_type = group.getField(FIELD::MDEntryType);
    if(entry_type == "0")
    { // Bid
      bid_price = DoubleConvertor::convert(group.getField(FIELD::MDEntryPx));
    }
    else if(entry_type == "1")
    { // Ask (Offer)
      ask_price = DoubleConvertor::convert(group.getField(FIELD::MDEntryPx));
    }
  }
  cout << "MarketDataSnapshotFullRefresh -> Symbol - " << symbol
       << " Bid - " << bid_price << " Ask - " << ask_price << endl;
}

void FixApp::onMessage(const FIX44::ExecutionReport& er, const SessionID& session_ID)
{
  cout << "ExecutionReport ->"
       << "\n ClOrdID -> " << er.getField(FIELD::ClOrdID)
       << "\n Account -> " << er.getField(FIELD::Account)
       << "\n OrderID -> " << er.getField(FIELD::OrderID) 
       << "\n LastQty -> " << er.getField(FIELD::LastQty)
       << "\n CumQty -> " << er.getField(FIELD::CumQty)
       << "\n ExecType -> " << er.getField(FIELD::ExecType)
       << "\n OrdStatus -> " << er.getField(FIELD::OrdStatus)
       << endl;
}

void FixApp::StartSession(string &file)
{
  try
  {
    settings = new SessionSettings(file);
    store_factory = new FileStoreFactory(* settings);
    log_factory = new FileLogFactory(* settings);
    initiator = new SocketInitiator(* this, * store_factory, * settings, * log_factory/*Optional*/);
    initiator->start();
  }
  catch(ConfigError error)
  {
    cout << error.what() << endl;
  }
}

void FixApp::EndSession()
{
  initiator->stop();
  delete initiator;
  delete settings;
  delete store_factory;
  delete log_factory;
}

void FixApp::GetTradingStatus()
{
  FIX44::TradingSessionStatusRequest request;
  request.setField(TradSesReqID(NextRequestID()));
  request.setField(TradingSessionID("PRE-OPEN"));
  request.setField(SubscriptionRequestType(SubscriptionRequestType_SNAPSHOT));
  Session::sendToTarget(request, sessionID(false));
}

void FixApp::GetAccounts()
{
  FIX44::CollateralInquiry request;
  request.setField(CollInquiryID(NextRequestID()));
  request.setField(TradingSessionID("FXCM"));
  request.setField(SubscriptionRequestType(SubscriptionRequestType_SNAPSHOT));
  Session::sendToTarget(request, sessionID(false));
}

void FixApp::GetPositions()
{
  int total_accounts = (int)list_accountID.size();
  for(int i = 0; i < total_accounts; i++)
  {
    string accountID = list_accountID.at(i);
    FIX44::RequestForPositions request;
    request.setField(PosReqID(NextRequestID()));
    request.setField(PosReqType(PosReqType_POSITIONS));
    request.setField(Account(accountID));
    request.setField(SubscriptionRequestType(SubscriptionRequestType_SNAPSHOT));
    // AccountType_ACCOUNT_IS_CARRIED_ON_NON_CUSTOMER_SIDE_OF_BOOKS_AND_IS_CROSS_MARGINED
    request.setField(AccountType(6));
    request.setField(TransactTime());
    request.setField(ClearingBusinessDate());
    request.setField(TradingSessionID("FXCM"));
    request.setField(NoPartyIDs(1));
    FIX44::RequestForPositions::NoPartyIDs parties_group;
    parties_group.setField(PartyID("FXCM ID"));
    parties_group.setField(PartyIDSource('D'));
    parties_group.setField(PartyRole(3));
    parties_group.setField(NoPartySubIDs(1));
    FIX44::RequestForPositions::NoPartyIDs::NoPartySubIDs sub_parties;
    sub_parties.setField(PartySubIDType(PartySubIDType_SECURITIES_ACCOUNT_NUMBER));
    sub_parties.setField(PartySubID(accountID));
    parties_group.addGroup(sub_parties);
    request.addGroup(parties_group);
    Session::sendToTarget(request, sessionID(false));
  }
}

void FixApp::queryMarketDataRequest44()
{
  MDReqID mdReqID( "MARKETDATAID" );
  SubscriptionRequestType subType( SubscriptionRequestType_SNAPSHOT );
  MarketDepth marketDepth( 0 );
  FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  MDEntryType mdEntryType( MDEntryType_BID );
  marketDataEntryGroup.set( mdEntryType );

  FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
  Symbol symbol("EUR/USD");
  symbolGroup.set( symbol );

  FIX44::MarketDataRequest message( mdReqID, subType, marketDepth );
  message.addGroup( marketDataEntryGroup );
  message.addGroup( symbolGroup );

  queryHeader( message.getHeader() );

  cout << "[cuongth] request market data:\n"
       << message.toXML() << '\n'
       << message.toString() << endl;

  Session::sendToTarget( message );
}

void FixApp::SubscribeMarketData()
{
  string request_ID = "EUR_USD_Request_";
  FIX44::MarketDataRequest request;
  request.setField(MDReqID(request_ID));
  // SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES
  request.setField(SubscriptionRequestType('1'));
  request.setField(MarketDepth(0));
  request.setField(NoRelatedSym(1));

  FIX44::MarketDataRequest::NoRelatedSym symbols_group;
  symbols_group.setField(Symbol("EUR/USD"));
  request.addGroup(symbols_group);

  FIX44::MarketDataRequest::NoMDEntryTypes entry_types;
  entry_types.setField(MDEntryType(MDEntryType_BID));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_OFFER));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_TRADING_SESSION_HIGH_PRICE));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_TRADING_SESSION_LOW_PRICE));
  request.addGroup(entry_types);

  Session::sendToTarget(request, sessionID(true));
}

void FixApp::UnsubscribeMarketData()
{
  string request_ID = "EUR_USD_Request_";
  FIX44::MarketDataRequest request;
  request.setField(MDReqID(request_ID));
  // SubscriptionRequestType_DISABLE_PREVIOUS_SNAPSHOT_PLUS_UPDATE_REQUEST
  request.setField(SubscriptionRequestType('2'));
  request.setField(MarketDepth(0));
  request.setField(NoRelatedSym(1));

  FIX44::MarketDataRequest::NoRelatedSym symbols_group;
  symbols_group.setField(Symbol("EUR/USD"));
  request.addGroup(symbols_group);

  FIX44::MarketDataRequest::NoMDEntryTypes entry_types;
  entry_types.setField(MDEntryType(MDEntryType_BID));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_OFFER));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_TRADING_SESSION_HIGH_PRICE));
  request.addGroup(entry_types);
  entry_types.setField(MDEntryType(MDEntryType_TRADING_SESSION_LOW_PRICE));
  request.addGroup(entry_types);

  Session::sendToTarget(request, sessionID(true));
}

void FixApp::MarketOrder()
{
  int total_accounts = (int)list_accountID.size();
  for(int i = 0; i < total_accounts; i++)
  {
    string accountID = list_accountID.at(i);
    FIX44::NewOrderSingle request;
    request.setField(ClOrdID(NextRequestID()));
    request.setField(Account(accountID));
    request.setField(Symbol("EUR/USD"));
    request.setField(TradingSessionID("FXCM"));
    request.setField(TransactTime());
    request.setField(OrderQty(10000));
    request.setField(Side(FIX::Side_BUY));
    request.setField(OrdType(OrdType_MARKET));
    request.setField(TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));
    Session::sendToTarget(request, sessionID(false));
  }
}

string FixApp::NextRequestID()
{
  if(requestID == 65535)
    requestID = 1;
  requestID++;
  string next_ID = IntConvertor::convert(requestID);
  return next_ID;
}

void FixApp::RecordAccount(string accountID)
{
  int size = (int)list_accountID.size();
  if(size == 0)
  {
    list_accountID.push_back(accountID);
  }
  else
  {
    for(int i = 0; i < size; i++)
    {
      if(list_accountID.at(i) == accountID)
        break;
      if(i == size - 1)
        list_accountID.push_back(accountID);
    }
  }
}

void FixApp::queryHeader( FIX::Header& header )
{
  header.setField( querySenderCompID() );
  header.setField( queryTargetCompID() );

  if ( queryConfirm( "Use a TargetSubID" ) )
    header.setField( queryTargetSubID() );
}

SenderCompID FixApp::querySenderCompID()
{
  std::string value;
  std::cout << std::endl << "SenderCompID: ";
  std::cin >> value;
  return SenderCompID( value );
}

TargetCompID FixApp::queryTargetCompID()
{
  std::string value;
  std::cout << std::endl << "TargetCompID: ";
  std::cin >> value;
  return TargetCompID( value );
}

TargetSubID FixApp::queryTargetSubID()
{
  std::string value;
  std::cout << std::endl << "TargetSubID: ";
  std::cin >> value;
  return TargetSubID( value );
}

bool FixApp::queryConfirm( const string& query )
{
  string value;
  std::cout << std::endl << query << "?: ";
  std::cin >> value;
  return toupper( *value.c_str() ) == 'Y';
}
