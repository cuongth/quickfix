#include "FixApp.h"

int main(int argc, char** argv)
{
  if ( argc < 2 )
  {
    cout << "usage: " << argv[ 0 ]
         << " FILE." << endl;
    return 0;
  }
  string file = argv[ 1 ];

  FixApp app;
  app.StartSession(file);

  while(true)
  {
    int command = 0;
    bool exit = false;
    cout << "Enter:\n 0 to exit\n"
         << " 1 to get positions\n"
         << " 2 to subscribe market data\n"
         << " 3 to unsubscribe market data\n"
         << " 4 to send marke order\n"
         << " 5 request market data of GBP/USD.\n"
         << " 6 unsubscribe market data of GBP/USD.\n";
    cin >> command;

    switch(command)
    {
      case 0: // Exit example application
        exit = true;
        break;
      case 1: // Get positions
        app.GetPositions();
        break;
      case 2: // Subscribe to market data
        app.SubscribeMarketData();
        break;
      case 3: // Unsubscribe to market data
        app.UnsubscribeMarketData();
        break;
      case 4: // Send market order
        app.MarketOrder();
        break;
      case 5: // Request market data
        app.queryMarketDataRequest(true);
        break;
      case 6: // unsubscribe market data
        app.queryMarketDataRequest(false);
        break;
    }
    if(exit)
      break;
  }

  app.EndSession();
  return 0;
}
