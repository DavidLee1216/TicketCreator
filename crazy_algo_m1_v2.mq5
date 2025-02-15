//+------------------------------------------------------------------+
//|                                               auto_trader_m5.mq5 |
//|                                                        David Lee |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "David Lee"
#property link      "https://www.mql5.com"
#property version   "1.00"
#include <Trade/Trade.mqh>
#include <Trade/PositionInfo.mqh>
#define BUTTON_CLOSE_ALL "CloseAllButton"
#define BUTTON_CLOSE_PROFIT "CloseAllProfitButton"
#define BUTTON_CLOSE_NONPROFIT "CloseAllNonProfitButton"
#define BUTTON_CLOSE_SELL "CloseSellPositionsButton"
#define BUTTON_CLOSE_BUY "CloseBuyPositionsButton"
#define BUTTON_CLOSE_ONE_BUY "CloseOneBuyPositionButton"
#define BUTTON_CLOSE_ONE_SELL "CloseOneSellPositionButton"
#define BUTTON_BUY "BuyPositionButton"
#define BUTTON_SELL "SellPositionButton"
#define BUTTON_SPECIAL_BUY_SHORT "BuySpecialPositionButtonShort"
#define BUTTON_SPECIAL_SELL_SHORT "SellSpecialPositionButtonShort"
#define BUTTON_SPECIAL_BUY_LONG "BuySpecialPositionButtonLong"
#define BUTTON_SPECIAL_SELL_LONG "SellSpecialPositionButtonLong"
#define ANALYZE_LABEL1 "AnalyzeLabel1"
#define ANALYZE_LABEL2 "AnalyzeLabel2"
#define ANALYZE_LABEL3 "AnalyzeLabel3"
#define ARROW_UP_DOWN "ArrowUpDown"
#define BUY_SELL_MARK "BuySellMark"

#resource "\\Indicators\\Examples\\ZigZag.ex5";

enum CloseKind
{
   MacdCloseKind,
   ZigzagCloseKind,
   LeapCloseKind,
   MaxMinCloseKind,
   LongTermSpecialCloseShortKind,
   LongTermSpecialCloseLongKind,
   AmaTemaCloseKind,
   AmaTemaNextCloseKind,
   Macd15CloseKind,
};

enum SignalPattern
{
   SellSignalPattern=-1,
   BuySignalPattern=1,
   SellCloseSignalPattern=-2,
   BuyCloseSignalPattern=2,
   SellSignalPattern15=-3,
   BuySignalPattern15 = 3,
   SellCloseSignalPattern15=-4,
   BuyCloseSignalPattern15=4,
   NoSignalPattern=0
};

enum TrendPattern
{
   TrendUp=1,
   TrendDown=-1,
   NoTrend=0
};

enum TrendPotentialPattern
{
   PotentialTrendUp,
   PotentialTrendDown,
   NoPotentialTrend
};

enum VolatilityType
{
   Large,
   Small,
   Stable
};

enum CandlePattern
{
   LongMarubozu,
   ShortMarubozu,
   BullishHammer,
   BearishHammer,
   SpinningTop,
   Unknown
};
bool m15 = false;
input int      long_term_trend=2;//external long term trend, 0:notrend,1:up, -1:down, 2:dont use
input double   spread = 20.0;
input bool orderEnabled = true;
input bool specialTrading =false;
bool minmaxTrading = false;
input bool macdTrading = false;
input bool macd15Trading = false;
input bool amatemaTrading = false;
input bool leapTrading = true;
input double upper_price = 62000;
input double lower_price = 57000;
input double autoLots = 0.1;
input double myLots = 0.2; //default lots
input double specialLots = 0.5;
input bool handBidAutoClose = true;
input bool autoBidAutoClose = true;
input bool handBidAutoConsistencyClose = true;
input bool handBidAutoBid = false;
input bool autoBidAutoConsistencyClose = true;
input double stopLossAutoDefault = 100;
input double stopLossHandDefault = 100;
input double profitBackStopLoss = 50;
input bool handBidAutoConstProfitClose = false;
input bool acceptStopLoss = false;
input bool atrStoploss = false;
input double g_target_profit = 500;
input double g_target_profit_s = 100;

input int bidLimitInTrack = 1;
input int bidLimitInTrend = 1;

bool autoConsistencyBid = false;
bool placeOrderWhenClose = false;
bool temaClose = false;
bool stdClose = false;


input double takeProfitDefault = 500;

bool     bPrintShow=false;
bool     bPrintShowz=true;
bool bOrderEnabled = true;
bool bNoBidFor15Min = false;
bool bNoBidForOneMin = false;

datetime noBidTime = 0;
double resistance_price = upper_price;
double support_price = lower_price;
double resistance_price2 = upper_price;
double support_price2 = lower_price;
datetime resistance_time = 0;
datetime support_time = 0;
datetime resistance_time2 = 0;
datetime support_time2 = 0;
int trading_start_hour[2] = {1, 8};
int trading_end_hour[2] = {7, 22};
double limitThreshold = 10;
double leapThreshold = 80;
double leapMinimumLastStep = 10;
double amaStepThreshold = 3;
double temaThreshold = 1;
double macdThreshold = 3;
double amatemaDiffThreshold = 2;
int defaultAfterTickCountInBar=3;
int defaultAfterBarCountToCheck=1200;
double leapBarLenThreshold = 120;
int longTermSpecialCountLimit = 1;
double stopLossLeap = 100;
double prev_average_tick_volume = 0;
double recent_average_tick_volume = 0;
datetime average_record_start_time = 0;
int five_min_tick_count = 0;

double g_ask = 0;
double g_bid = 0;
double g_prev_ask = 0;
double g_prev_bid = 0;

string _symbol = _Symbol;
double TEMABuffer[], TEMABuffer15[], TEMABuffer5[], MACDBuffer[], MACDBuffer5[], MACDBuffer15[], MACDSignalBuffer[], MACDSignalBuffer5[], MACDSignalBuffer15[], ZIGZAGBuffer[], ZigzagHighBuffer[], 
   ZigzagLowBuffer[], AmaBuffer[], AmaBuffer5[], AmaBuffer15[], AdxBuffer[], AdxPDIBuffer[], AdxNDIBuffer[], RsiBuffer[], StdDevBuffer[], AtrBuffer[], StdDevBuffer15[], AtrBuffer15[], BBUpperBuffer[],
   BBLowerBuffer[], BBMiddleBuffer[], AdxBuffer5[], AdxPDIBuffer5[], AdxNDIBuffer5[];
int handleTEMA, handleTEMA5, handleTEMA15, handleTEMA60, handleMACD, handleMACD5, handleMACD15, handleZigzag, handleAMA, handleAMA5, handleAMA15, handleADX, handleADX5, handleRSI, handleStdDev, handleAtr, 
   handleStdDev15, handleAtr15, handleBollingerBand;
int highIndex[10], lowIndex[10];

int TEMA_INP_PERIOD_EMA = 10;
int TEMA_INP_SHIFT = 0;

int AMA_PERIOD = 6;
int AMA_FAST_PERIOD_EMA = 2;
int AMA_SLOW_PERIOD_EMA = 30;
int AMA_SHIFT = 0;

int ZIGZAG_DEPTH = 5;
int ZIGZAG_DEVIATION = 5;
int ZIGZAG_BACKSTEP = 3;

int MACD_FAST_EMA_PERIOD = 12;
int MACD_SLOW_EMA_PERIOD = 26;
int MACD_SIGNAL_PERIOD = 9;

double g_max_price = 0;
double g_min_price = 0;
double g_mean_price = 0;
int g_bidCountInTrack = 0;
int g_bidCountInTrend = 0;
int g_bidCountInTrack5 = 0;

CTrade trade;
COrderInfo     g_order; //Library for Orders information
CPositionInfo  g_position; // Library for all position features and information

TrendPattern zigzagTrend = NoTrend;
TrendPattern amaTrend = NoTrend;
TrendPattern rsiTrend = NoTrend;
TrendPattern adxTrend = NoTrend;
TrendPattern temaTrend = NoTrend;
TrendPattern temaTrend15 = NoTrend;
TrendPattern temaTrend5 = NoTrend;
TrendPattern longTemaTrend = NoTrend;
TrendPattern volumeTrend = NoTrend;
TrendPotentialPattern potentialTrend = NoPotentialTrend;

datetime potential_trend_time = 0;
SignalPattern amatemaSignal = NoSignalPattern;
SignalPattern macdSignal = NoSignalPattern;
SignalPattern macdSignal15 = NoSignalPattern;
bool buy_sell_color_toggle = true;
bool amatemaNextSignal = false;
bool amatemaAgain = false;
datetime amatemaLastTick = 0;
datetime consistencyClosedTime = 0;
double consistencyCloseLot = 0;

class AutoTradedTickets;
class ChartAnalyzer;

AutoTradedTickets* autoTradedTickets;
ChartAnalyzer* chartAnalyzer;

int OnInit()
{
   if(m15)
      ChartSetSymbolPeriod(0, _symbol, PERIOD_M15);
   else
      ChartSetSymbolPeriod(0, _symbol, PERIOD_M1);
   average_record_start_time = iTime(_symbol, PERIOD_M5, 0);
   EventSetMillisecondTimer(200);
   string currentSymbol = Symbol();
   autoTradedTickets = new AutoTradedTickets();
   chartAnalyzer = new ChartAnalyzer();
   handleAMA15 = iAMA(_symbol, PERIOD_M15, AMA_PERIOD, AMA_FAST_PERIOD_EMA, AMA_SLOW_PERIOD_EMA, AMA_SHIFT, PRICE_CLOSE);
   handleAMA5 = iAMA(_symbol, PERIOD_M5, AMA_PERIOD, AMA_FAST_PERIOD_EMA, AMA_SLOW_PERIOD_EMA, AMA_SHIFT, PRICE_CLOSE);
   handleAMA = iAMA(_symbol, PERIOD_M1, AMA_PERIOD, AMA_FAST_PERIOD_EMA, AMA_SLOW_PERIOD_EMA, AMA_SHIFT, PRICE_CLOSE);
   handleZigzag = iCustom(_symbol, PERIOD_M1, "::Indicators\\Examples\\ZigZag.ex5", ZIGZAG_DEPTH, ZIGZAG_DEVIATION, ZIGZAG_BACKSTEP);
   handleTEMA = iTEMA(_symbol, PERIOD_M1, 10, 0, PRICE_CLOSE);
   handleTEMA5 = iTEMA(_symbol, PERIOD_M5, 10, 0, PRICE_CLOSE);
   handleTEMA15 = iTEMA(_symbol, PERIOD_M15, 10, 0, PRICE_CLOSE);
   handleTEMA60 = iTEMA(_symbol, PERIOD_H1, 5, 0, PRICE_CLOSE);
   handleMACD = iMACD(_symbol, PERIOD_CURRENT, MACD_FAST_EMA_PERIOD, MACD_SLOW_EMA_PERIOD, MACD_SIGNAL_PERIOD, PRICE_CLOSE);
   handleMACD5 = iMACD(_symbol, PERIOD_M5, MACD_FAST_EMA_PERIOD, MACD_SLOW_EMA_PERIOD, MACD_SIGNAL_PERIOD, PRICE_CLOSE);
   handleMACD15 = iMACD(_symbol, PERIOD_M15, MACD_FAST_EMA_PERIOD, MACD_SLOW_EMA_PERIOD, MACD_SIGNAL_PERIOD, PRICE_CLOSE);
   handleRSI = iRSI(_symbol, PERIOD_M1, 14, PRICE_CLOSE);
   handleADX = iADX(_symbol, PERIOD_M1, 14);
   handleADX5 = iADX(_symbol, PERIOD_M5, 14);
   handleStdDev = iStdDev(_symbol, PERIOD_M1, 14, 0, MODE_SMA, PRICE_CLOSE);
   handleAtr = iATR(_symbol, PERIOD_M1, 14);
   handleStdDev15 = iStdDev(_symbol, PERIOD_M5, 14, 0, MODE_SMA, PRICE_CLOSE);
   handleAtr15 = iATR(_symbol, PERIOD_M5, 14);
   handleBollingerBand = iBands(_symbol, PERIOD_M1, 14, 0, 2, PRICE_CLOSE);
   createButtons();
   createLabels();
   ArraySetAsSeries(MACDBuffer, true);
   ArraySetAsSeries(MACDBuffer15, true);
   ArraySetAsSeries(MACDBuffer5, true);
   ArraySetAsSeries(MACDSignalBuffer, true);
   ArraySetAsSeries(MACDSignalBuffer15, true);
   ArraySetAsSeries(MACDSignalBuffer5, true);
   ArraySetAsSeries(TEMABuffer, true);
   ArraySetAsSeries(TEMABuffer15, true);
   ArraySetAsSeries(TEMABuffer5, true);
   ArraySetAsSeries(ZIGZAGBuffer, true);
   ArraySetAsSeries(ZigzagHighBuffer, true);
   ArraySetAsSeries(ZigzagLowBuffer, true);
   ArraySetAsSeries(AmaBuffer, true);
   ArraySetAsSeries(AmaBuffer5, true);
   ArraySetAsSeries(AmaBuffer15, true);
   ArraySetAsSeries(AdxBuffer, true);
   ArraySetAsSeries(AdxPDIBuffer, true);
   ArraySetAsSeries(AdxNDIBuffer, true);
   ArraySetAsSeries(AdxBuffer5, true);
   ArraySetAsSeries(AdxPDIBuffer5, true);
   ArraySetAsSeries(AdxNDIBuffer5, true);
   ArraySetAsSeries(RsiBuffer, true);
   ArraySetAsSeries(StdDevBuffer, true);
   ArraySetAsSeries(AtrBuffer, true);
   ArraySetAsSeries(StdDevBuffer15, true);
   ArraySetAsSeries(AtrBuffer15, true);
   ArraySetAsSeries(BBUpperBuffer, true);
   ArraySetAsSeries(BBLowerBuffer, true);
   ArraySetAsSeries(BBMiddleBuffer, true);
   if(handleMACD==INVALID_HANDLE || handleMACD5==INVALID_HANDLE || handleMACD15==INVALID_HANDLE || handleTEMA==INVALID_HANDLE || handleTEMA15==INVALID_HANDLE || handleTEMA5==INVALID_HANDLE|| handleZigzag==INVALID_HANDLE||
      handleAMA==INVALID_HANDLE||handleAMA5==INVALID_HANDLE||handleAMA15==INVALID_HANDLE||handleADX==INVALID_HANDLE || handleRSI==INVALID_HANDLE || handleStdDev==INVALID_HANDLE ||
       handleAtr==INVALID_HANDLE || handleStdDev15==INVALID_HANDLE || handleAtr15==INVALID_HANDLE || handleBollingerBand==INVALID_HANDLE
       )
      return(INIT_FAILED);
   return(INIT_SUCCEEDED);
}

void OnTimer()
{
   if(amatemaSignal==BuySignalPattern||macdSignal==BuySignalPattern||macdSignal15==BuySignalPattern)
   {
      if(buy_sell_color_toggle)
         ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrRed);
      else
         ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrGreenYellow);
      if(macdSignal15==BuySignalPattern)
         ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, "매수15");
      else
         ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, "매수");
   }
   else if(amatemaSignal==SellSignalPattern||macdSignal==SellSignalPattern||macdSignal15==SellSignalPattern)
   {
      if(buy_sell_color_toggle)
         ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrRed);
      else
         ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrGreenYellow);
      if(macdSignal15==SellSignalPattern)
         ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, "매도15");
      else
         ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, "매도");
   }
   else
   {
      ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrBlack);
      ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, " ");
   }
   buy_sell_color_toggle = !buy_sell_color_toggle;
}

bool createButtons()
{
   if(!ObjectCreate(0, BUTTON_CLOSE_ALL, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_CLOSE_PROFIT, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_CLOSE_NONPROFIT, OBJ_BUTTON, 0, 0, 0)||
      !ObjectCreate(0, BUTTON_CLOSE_SELL, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_CLOSE_BUY, OBJ_BUTTON, 0, 0, 0)||
      !ObjectCreate(0, BUTTON_CLOSE_ONE_BUY, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_CLOSE_ONE_SELL, OBJ_BUTTON, 0, 0, 0)||
      !ObjectCreate(0, BUTTON_SELL, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_BUY, OBJ_BUTTON, 0, 0, 0)||
      !ObjectCreate(0, BUTTON_SPECIAL_SELL_SHORT, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_SPECIAL_BUY_SHORT, OBJ_BUTTON, 0, 0, 0)||
      !ObjectCreate(0, BUTTON_SPECIAL_SELL_LONG, OBJ_BUTTON, 0, 0, 0)||!ObjectCreate(0, BUTTON_SPECIAL_BUY_LONG, OBJ_BUTTON, 0, 0, 0))
   {
      Print("Failed to create button!");
      return false;
   }
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_XDISTANCE, 300);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_ALL, OBJPROP_TEXT, "Close All");
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_COLOR, clrBlue);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_BGCOLOR,clrLightGreen); 
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_ZORDER, 0);
   ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_STATE, true);
   
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_XDISTANCE, 410);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_PROFIT, OBJPROP_TEXT, "Close Profitable");
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_BGCOLOR,clrRed); 
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_XDISTANCE, 520);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_TEXT, "Close Losing");
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_COLOR, clrBlack);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_BGCOLOR,clrLightSteelBlue); 
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_XDISTANCE, 850);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_SELL, OBJPROP_TEXT, "Close Sell");
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_BGCOLOR,clrOrangeRed); 
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_XDISTANCE, 960);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_BUY, OBJPROP_TEXT, "Close Buy");
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_BGCOLOR,clrRoyalBlue); 
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_XDISTANCE, 630);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_TEXT, "CloseOneSell");
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_BGCOLOR,clrBlack); 
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_XDISTANCE, 740);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_YDISTANCE, 30);
   ObjectSetString(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_TEXT, "CloseOneBuy");
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_COLOR, clrBlack);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_BGCOLOR,clrWhite); 
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_ZORDER, 0);


   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_XDISTANCE, 740);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_SELL, OBJPROP_TEXT, "SELL");
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_BGCOLOR,clrRed); 
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_SELL, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_XDISTANCE, 850);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_BUY, OBJPROP_TEXT, "BUY");
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_BGCOLOR,clrBlue); 
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_BUY, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_XDISTANCE, 300);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_TEXT, "ShortSELL");
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_BGCOLOR,clrRed); 
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_XDISTANCE, 410);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_TEXT, "ShortBUY");
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_BGCOLOR,clrBlue); 
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_XDISTANCE, 520);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_TEXT, "LongSELL");
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_BGCOLOR,clrRed); 
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_ZORDER, 0);

   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_XSIZE, 100);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_YSIZE, 30);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_XDISTANCE, 630);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_YDISTANCE, 70);
   ObjectSetString(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_TEXT, "LongBUY");
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_COLOR, clrWhite);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_BGCOLOR,clrBlue); 
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_HIDDEN, false);
   ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_ZORDER, 0);

   return true;
}

bool createLabels()
{
   if(!ObjectCreate(0, ANALYZE_LABEL1, OBJ_LABEL, 0, 0, 0))
   {
      Print("레이블 객체 생성 실패: ", GetLastError());
      return false;
   }

   ObjectSetString(0, ANALYZE_LABEL1, OBJPROP_TEXT, "");
   ObjectSetInteger(0, ANALYZE_LABEL1, OBJPROP_COLOR, clrYellow);
   ObjectSetInteger(0, ANALYZE_LABEL1, OBJPROP_FONTSIZE, 12);
   ObjectSetInteger(0, ANALYZE_LABEL1, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, ANALYZE_LABEL1, OBJPROP_XDISTANCE, 10);
   ObjectSetInteger(0, ANALYZE_LABEL1, OBJPROP_YDISTANCE, 250);

   if(!ObjectCreate(0, ANALYZE_LABEL2, OBJ_LABEL, 0, 0, 0))
   {
      Print("레이블 객체 생성 실패: ", GetLastError());
      return false;
   }

   ObjectSetString(0, ANALYZE_LABEL2, OBJPROP_TEXT, "");
   ObjectSetInteger(0, ANALYZE_LABEL2, OBJPROP_COLOR, clrYellow);
   ObjectSetInteger(0, ANALYZE_LABEL2, OBJPROP_FONTSIZE, 12);
   ObjectSetInteger(0, ANALYZE_LABEL2, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, ANALYZE_LABEL2, OBJPROP_XDISTANCE, 10);
   ObjectSetInteger(0, ANALYZE_LABEL2, OBJPROP_YDISTANCE, 280);

   if(!ObjectCreate(0, ANALYZE_LABEL3, OBJ_LABEL, 0, 0, 0))
   {
      Print("레이블 객체 생성 실패: ", GetLastError());
      return false;
   }

   ObjectSetString(0, ANALYZE_LABEL3, OBJPROP_TEXT, "");
   ObjectSetInteger(0, ANALYZE_LABEL3, OBJPROP_COLOR, clrYellow);
   ObjectSetInteger(0, ANALYZE_LABEL3, OBJPROP_FONTSIZE, 12);
   ObjectSetInteger(0, ANALYZE_LABEL3, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, ANALYZE_LABEL3, OBJPROP_XDISTANCE, 10);
   ObjectSetInteger(0, ANALYZE_LABEL3, OBJPROP_YDISTANCE, 310);

   if(!ObjectCreate(0, ARROW_UP_DOWN, OBJ_LABEL, 0, 0, 0))
   {
      Print("Arrow 객체 생성 실패: ", GetLastError());
      return false;
   }
   ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_COLOR, clrGreenYellow);
   ObjectSetString(0 , ARROW_UP_DOWN, OBJPROP_FONT, "Wingdings");
   ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_FONTSIZE, 12);
   ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_XDISTANCE, 390);
   ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_YDISTANCE, 280);
   ObjectSetString(0, ARROW_UP_DOWN, OBJPROP_TEXT, CharToString(233));

   if(!ObjectCreate(0, BUY_SELL_MARK, OBJ_LABEL, 0, 0, 0))
   {
      Print("BuySell 객체 생성 실패: ", GetLastError());
      return false;
   }
   ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_COLOR, clrBlack);
   ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_FONTSIZE, 14);
   ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_CORNER, CORNER_LEFT_UPPER);
   ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_XDISTANCE, 420);
   ObjectSetInteger(0, BUY_SELL_MARK, OBJPROP_YDISTANCE, 280);
   ObjectSetString(0, BUY_SELL_MARK, OBJPROP_TEXT, "");

   return true;
}

void OnDeinit(const int reason)
{
   ObjectDelete(0, BUTTON_CLOSE_ALL);
   ObjectDelete(0, BUTTON_CLOSE_PROFIT);
   ObjectDelete(0, BUTTON_CLOSE_NONPROFIT);
   ObjectDelete(0, BUTTON_CLOSE_SELL);
   ObjectDelete(0, BUTTON_CLOSE_BUY);
   ObjectDelete(0, BUTTON_CLOSE_ONE_BUY);
   ObjectDelete(0, BUTTON_CLOSE_ONE_SELL);
   ObjectDelete(0, BUTTON_SELL);
   ObjectDelete(0, BUTTON_BUY);
   ObjectDelete(0, BUTTON_SPECIAL_SELL_SHORT);
   ObjectDelete(0, BUTTON_SPECIAL_BUY_SHORT);
   ObjectDelete(0, BUTTON_SPECIAL_SELL_LONG);
   ObjectDelete(0, BUTTON_SPECIAL_BUY_LONG);
   
   ObjectDelete(0, ANALYZE_LABEL1);
   ObjectDelete(0, ANALYZE_LABEL2);
   ObjectDelete(0, ANALYZE_LABEL3);
   ObjectDelete(0, ARROW_UP_DOWN);
   ObjectDelete(0, BUY_SELL_MARK);
   
   IndicatorRelease(handleMACD);
   IndicatorRelease(handleMACD5);
   IndicatorRelease(handleMACD15);
   IndicatorRelease(handleTEMA);
   IndicatorRelease(handleTEMA15);
   IndicatorRelease(handleTEMA5);
   IndicatorRelease(handleZigzag);
   IndicatorRelease(handleAMA);
   IndicatorRelease(handleAMA5);
   IndicatorRelease(handleAMA15);
   IndicatorRelease(handleADX);
   IndicatorRelease(handleADX5);
   IndicatorRelease(handleRSI);
   IndicatorRelease(handleStdDev);
   IndicatorRelease(handleAtr);
   IndicatorRelease(handleStdDev15);
   IndicatorRelease(handleAtr15);
   IndicatorRelease(handleBollingerBand);
   
   ObjectsDeleteAll(0);
   EventKillTimer();
   delete autoTradedTickets;
   delete chartAnalyzer;
}

void OnChartEvent(const int id,
                  const long &lparam,
                  const double &dparam,
                  const string &sparam)
{
   // 버튼 클릭 이벤트인지 확인
   if(id==CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_ALL)
   {
      // 버튼이 클릭되었을 때의 동작 수행
      CloseAllPositions();
      ObjectSetInteger(0, BUTTON_CLOSE_ALL, OBJPROP_STATE, false);
   }
   else if(id==CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_PROFIT)
   {
      CloseAllProfitable();
      ObjectSetInteger(0, BUTTON_CLOSE_PROFIT, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_NONPROFIT)
   {
      CloseAllNonProfitable();
      ObjectSetInteger(0, BUTTON_CLOSE_NONPROFIT, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_SELL)
   {
      CloseSellPositions();
      ObjectSetInteger(0, BUTTON_CLOSE_SELL, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_BUY)
   {
      CloseBuyPositions();
      ObjectSetInteger(0, BUTTON_CLOSE_BUY, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_ONE_BUY)
   {
      CloseOneBuyPosition();
      ObjectSetInteger(0, BUTTON_CLOSE_ONE_BUY, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_CLOSE_ONE_SELL)
   {
      CloseOneSellPosition();
      ObjectSetInteger(0, BUTTON_CLOSE_ONE_SELL, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_SELL)
   {
      SellOnePosition();
      ObjectSetInteger(0, BUTTON_SELL, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_BUY)
   {
      BuyOnePosition();
      ObjectSetInteger(0, BUTTON_BUY, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_SPECIAL_SELL_SHORT)
   {
      SellOneSpecialPosition();
      ObjectSetInteger(0, BUTTON_SPECIAL_SELL_SHORT, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_SPECIAL_BUY_SHORT)
   {
      BuyOneSpecialPosition();
      ObjectSetInteger(0, BUTTON_SPECIAL_BUY_SHORT, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_SPECIAL_SELL_LONG)
   {
      SellOneSpecialPosition(false);
      ObjectSetInteger(0, BUTTON_SPECIAL_SELL_LONG, OBJPROP_STATE, false);
   }
   else if(CHARTEVENT_OBJECT_CLICK && sparam==BUTTON_SPECIAL_BUY_LONG)
   {
      BuyOneSpecialPosition(false);
      ObjectSetInteger(0, BUTTON_SPECIAL_BUY_LONG, OBJPROP_STATE, false);
   }
}
datetime currBarTime = 0;
double g_equity = 0;
double g_margin = 0;
double g_free_margin = 0;
bool start_average_record = false;
datetime prev_5min_bar_time = 0;

void OnTick()
{
   if(GetBuffers()==false)
      return;
      
   datetime currentDate = TimeCurrent();
   g_ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
   g_bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
   
   average_record_start_time = iTime(_symbol, PERIOD_M5, 0);
   if(double(currentDate-average_record_start_time)<1 && !start_average_record)
   {
      start_average_record = true;
   }
   if(double(currentDate-average_record_start_time)<1 && start_average_record && prev_5min_bar_time!=average_record_start_time)
   {
      prev_average_tick_volume = recent_average_tick_volume;
   }
   if(start_average_record)
   {
      if(average_record_start_time!=prev_5min_bar_time)
      {
         recent_average_tick_volume = 0;
         five_min_tick_count = 1;
      }
      else
         five_min_tick_count++;
      recent_average_tick_volume = (recent_average_tick_volume*(five_min_tick_count-1) + MathAbs(g_bid-g_prev_bid))/(double)five_min_tick_count;
   }
   currBarTime = RemoveSeconds(currentDate);
   int currentDay = TimeDay(currentDate);
   int currentHour = TimeHour(currentDate);

   g_max_price = iHigh(_symbol, PERIOD_D1, 0);
   g_min_price = iLow(_symbol, PERIOD_D1, 0);
   
   g_equity = autoTradedTickets.getCurrEquity();
   g_margin = autoTradedTickets.getMargin();
   g_free_margin = autoTradedTickets.getFreeMargin();

   amaTrend = autoTradedTickets.DetermineTrendByAMA(AmaBuffer);
   temaTrend = CheckTEMATrend();
   temaTrend15 = CheckTEMA15Trend();
   temaTrend5 = CheckTEMA5Trend();
   zigzagTrend = autoTradedTickets.DetermineTrendByZigzag(ZIGZAGBuffer, ZigzagHighBuffer, ZigzagLowBuffer, highIndex, lowIndex);
   if(zigzagTrend!=NoTrend)
   {
      potentialTrend = NoPotentialTrend;
      potential_trend_time = 0;
   }
   longTemaTrend = CheckLongTemaTrend();
   rsiTrend = CheckRsiTrend();
   adxTrend = CheckAdxTrend();
   volumeTrend = CheckVolumeTrend();
   autoTradedTickets.addPriceToTenPrices(g_bid);
   chartAnalyzer.displayAnalyzeResult();
   bool bSpecial = false;
   SignalPattern signal_leap = NoSignalPattern, signal_macd = NoSignalPattern, signal_maxmin = NoSignalPattern, signal_longterm_special = NoSignalPattern, 
      signal_amatema = NoSignalPattern, signal_macd15 = NoSignalPattern;
   signal_longterm_special = autoTradedTickets.getStableSignalPattern();
   if(signal_longterm_special==NoSignalPattern)
   {
      if(leapTrading)
         signal_leap = autoTradedTickets.getLeapPattern();
      if(signal_leap==NoSignalPattern)
      {
         if(signal_leap==NoSignalPattern)
         {
            signal_macd = autoTradedTickets.getMacdSignalPattern();
            macdSignal = signal_macd;
            macdSignal15 = autoTradedTickets.getMacd15SignalPattern();
            signal_macd15 = macdSignal15;
            if(macdTrading==false && signal_macd!=2 && signal_macd!=-2)
            {
               signal_macd = NoSignalPattern;
            }
            if(macd15Trading==false)
               signal_macd15 = NoSignalPattern;
            if(amatemaTrading)
               signal_amatema = autoTradedTickets.getAmaTemaSignalPattern();
            amatemaSignal = signal_amatema;
            if(signal_macd==NoSignalPattern && amatemaTrading)
            {
               if(signal_amatema!=NoSignalPattern){
                  amatemaLastTick = iTime(_symbol, PERIOD_CURRENT, 0);
               }
            }
            else if(amatemaTrading==false)
            {
               signal_amatema = NoSignalPattern;
            }
            if(signal_macd==NoSignalPattern && signal_amatema==NoSignalPattern && minmaxTrading)
               signal_maxmin = autoTradedTickets.getMaxMinSignalPattern(ZIGZAGBuffer, ZigzagHighBuffer, ZigzagLowBuffer, highIndex, lowIndex);
         }
      }
   }
   SignalPattern signal = (signal_longterm_special!=NoSignalPattern)?signal_longterm_special:(signal_leap!=NoSignalPattern)?
      signal_leap:(signal_macd!=NoSignalPattern)?signal_macd:(signal_macd15!=NoSignalPattern)?signal_macd15:(signal_maxmin!=NoSignalPattern)?
      signal_maxmin:(signal_amatema!=NoSignalPattern)?signal_amatema:NoSignalPattern;//signal_zigzag;
//         PrintFormat("orderEnabled=%d, bOrderEnabled=%d", orderEnabled, bOrderEnabled);
   if(signal!=NoSignalPattern)
   {
      string signal_kind = (signal_longterm_special!=NoSignalPattern)?"signal_longterm_special":(signal_leap!=NoSignalPattern)?"signal_leap":(signal_macd!=NoSignalPattern)?
         "signal_macd":(signal_macd15!=NoSignalPattern)?"signal_macd15":(signal_maxmin!=NoSignalPattern)?"signal_maxmin":
         (!amatemaNextSignal)?"signal_amatema":"signal_amatema_next";//"signal_zigzag";
   if(signal!=2&&signal!=-2&&signal!=4&&signal!=-4)
   PrintFormat("signal=%d, kind=%s", signal, signal_kind);
      bool ret = 0;
      double price = (signal==BuySignalPattern)?g_ask:g_bid;
      CloseKind closeKind = (signal_longterm_special!=NoSignalPattern)?LongTermSpecialCloseLongKind:(signal_leap!=NoSignalPattern)?
         LeapCloseKind:(signal_macd!=NoSignalPattern)?MacdCloseKind:(signal_macd15!=NoSignalPattern)?Macd15CloseKind:(signal_maxmin!=NoSignalPattern)?
         MaxMinCloseKind:(!amatemaNextSignal)?AmaTemaCloseKind:AmaTemaNextCloseKind;//ZigzagCloseKind;
      double lot = (closeKind==LongTermSpecialCloseLongKind||specialTrading)?specialLots:autoLots;
      lot = ((signal==BuySignalPattern15||signal==SellSignalPattern15)&&g_bidCountInTrack>1)?specialLots:lot;
      
/*      if((temaTrend15==TrendUp && volumeTrend==TrendUp && signal==BuySignalPattern && g_bid>resistance_price-50)||
            (temaTrend15==TrendDown && volumeTrend==TrendDown && signal==SellSignalPattern && g_bid<support_price+50))
         lot = specialLots;*/
      if((signal==BuySignalPattern||signal==BuySignalPattern15) && orderEnabled && bOrderEnabled)
      {//AtrBuffer[0]*2
         double stop = ((autoBidAutoClose&&g_bidCountInTrack>=0)&&!m15)?g_ask-stopLossAutoDefault:((autoBidAutoClose||g_bidCountInTrack>1)&&m15)?g_ask-stopLossAutoDefault:0;//AtrBuffer[0]*1.5
         if(atrStoploss)
            stop = ((autoBidAutoClose&&g_bidCountInTrack>=0)&&!m15)?g_ask-AtrBuffer[0]*1.5:((autoBidAutoClose||g_bidCountInTrack>1)&&m15)?g_ask-AtrBuffer[0]*2:0;//AtrBuffer[0]*1.5
         if(closeKind==MaxMinCloseKind)
            stop = ((autoBidAutoClose&&g_bidCountInTrack>=0))?g_ask-stopLossAutoDefault:0;
         if(closeKind==MacdCloseKind)
            stop = MathMin(support_price2-spread, stop);
         if(signal_macd15!=NoSignalPattern)
            stop = g_ask-stopLossAutoDefault;
         double tp = g_ask+takeProfitDefault;
         if(signal_macd15!=NoSignalPattern)
            tp = g_ask+500;
//         double tp = (handBidAutoClose&&m15)?g_ask+AtrBuffer[0]*2:0;
/*         if(closeKind==LeapCloseKind||autoTradedTickets.checkLeapCandle())
            stop = 0;*/
         if(closeKind==MaxMinCloseKind&&potentialTrend!=NoPotentialTrend)
            stop = g_ask-200;
         if((currentHour>=trading_start_hour[0]&&currentHour<=trading_end_hour[0])||(currentHour>=trading_start_hour[1]&&currentHour<=trading_end_hour[1]))
         {
            if(closeKind==LongTermSpecialCloseLongKind && autoTradedTickets.m_LongSpecialCount<longTermSpecialCountLimit)
            {
               ret = trade.Buy(lot, _symbol, price, stop, tp);
               if(ret)
                  autoTradedTickets.m_LongSpecialCount++;
            }
            else if(closeKind!=LongTermSpecialCloseLongKind && specialTrading)
               ret = trade.Buy(lot, _symbol, price, stop, tp);
            else if(closeKind!=LongTermSpecialCloseLongKind && !specialTrading)
               ret = trade.Buy(lot, _symbol, price, stop, tp);
         }
         else
         {
            CloseAllProfitable();
         }
      }
      else if((signal==SellSignalPattern||signal==SellSignalPattern15) && orderEnabled && bOrderEnabled)
      {//AtrBuffer[0]*2
         double stop = ((autoBidAutoClose&&g_bidCountInTrack>=0)&&!m15)?g_bid+stopLossAutoDefault:((autoBidAutoClose||g_bidCountInTrack>1)&&m15)?g_bid+stopLossAutoDefault:0;//AtrBuffer[0]*1.5
         if(atrStoploss)
            stop = ((autoBidAutoClose&&g_bidCountInTrack>=0)&&!m15)?g_bid+AtrBuffer[0]*1.5:((autoBidAutoClose||g_bidCountInTrack>1)&&m15)?g_bid+AtrBuffer[0]*2:0;//AtrBuffer[0]*1.5
         if(closeKind==MaxMinCloseKind)
            stop = ((autoBidAutoClose&&g_bidCountInTrack>=0))?g_bid+stopLossAutoDefault:0;
         if(closeKind==MacdCloseKind)
            stop = MathMin(resistance_price2+spread, stop);
         if(signal_macd15!=NoSignalPattern)
            stop = g_bid+stopLossAutoDefault;
         double tp = g_bid-takeProfitDefault;
         if(signal_macd15!=NoSignalPattern)
            tp = g_bid-500;
//         double tp = (handBidAutoClose&&m15)?g_bid-AtrBuffer[0]*2:0;
/*         if(closeKind==LeapCloseKind||autoTradedTickets.checkLeapCandle())
            stop = 0;*/
         if(closeKind==MaxMinCloseKind&&potentialTrend!=NoPotentialTrend)
            stop = g_bid+200;
         if((currentHour>=trading_start_hour[0]&&currentHour<=trading_end_hour[0])||(currentHour>=trading_start_hour[1]&&currentHour<=trading_end_hour[1]))
         {
            if(closeKind==LongTermSpecialCloseLongKind && autoTradedTickets.m_LongSpecialCount<longTermSpecialCountLimit)
            {
               ret = trade.Sell(lot, _symbol, price, stop, tp);
               if(ret)
                  autoTradedTickets.m_LongSpecialCount++;
            }
            else if(closeKind!=LongTermSpecialCloseLongKind && specialTrading)
               ret = trade.Sell(lot, _symbol, price, stop, tp);
            else if(closeKind!=LongTermSpecialCloseLongKind && !specialTrading)
               ret = trade.Sell(lot, _symbol, price, stop, tp);
         }
         else
         {
            CloseAllProfitable();
         }
      }
/*      else if(signal==BuyCloseSignalPattern)
         CloseBuyPositions(true);
      else if(signal==SellCloseSignalPattern)
         CloseSellPositions(true);
      else if(signal==BuyCloseSignalPattern15)
         CloseBuyPositions15();
      else if(signal==SellCloseSignalPattern15)
         CloseSellPositions15();*/
      if(ret)
      {
         double minmax_target_price = autoTradedTickets.getMinMaxTargetPrice(signal);
         if(closeKind != MaxMinCloseKind)
            autoTradedTickets.addMatchedSignal(signal, g_ask, g_bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*2);
         else
            autoTradedTickets.addMatchedSignal(signal, g_ask, g_bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, minmax_target_price);
         SignalPattern closePattern = (signal==BuySignalPattern)?SellSignalPattern:BuySignalPattern;
         if(closeKind!=MacdCloseKind && closeKind!=AmaTemaCloseKind && closeKind!=AmaTemaNextCloseKind)
            autoTradedTickets.CloseOrderPositions(closePattern, true);
         else
            autoTradedTickets.CloseOrderPositions(closePattern, false);
      }
   }
   autoTradedTickets.CloseWrongPositions();
   autoTradedTickets.CloseLongSpecialPosition();
   autoTradedTickets.recalcMatchedSignals();
   autoTradedTickets.setStopLoss();
   if(acceptStopLoss)
      autoTradedTickets.closePositionsUsingStopLoss();
   if(bPrintShow)
      printMatchedSignals();
   g_prev_ask = g_ask;
   g_prev_bid = g_bid;
   prev_5min_bar_time = average_record_start_time;
   if(autoConsistencyBid)
      autoTradedTickets.bidAgainConsistencyPattern();
}

void OnTrade()
{
}

void OnTradeTransaction(const MqlTradeTransaction& trans,
                        const MqlTradeRequest& request,
                        const MqlTradeResult& result)
{
}

void printMatchedSignals()
{
   string msg = StringFormat("%d, ticket = ", autoTradedTickets.m_nMatchedSignalsCount);
   for(int i = 0; i < autoTradedTickets.m_nMatchedSignalsCount; i++)
   {
      string ss = StringFormat("%d, ", autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
      msg = msg + ss;
   }
   Print(msg);
}

bool GetBuffers()
{
   if(CopyBuffer(handleMACD, 0, 0, 5, MACDBuffer)<=0||CopyBuffer(handleMACD15, 0, 0, 5, MACDBuffer15)<=0||CopyBuffer(handleMACD5, 0, 0, 5, MACDBuffer5)<=0||
      CopyBuffer(handleMACD, 1, 0, 5, MACDSignalBuffer)<=0||CopyBuffer(handleMACD5, 1, 0, 5, MACDSignalBuffer5)<=0||CopyBuffer(handleMACD15, 1, 0, 5, MACDSignalBuffer15)<=0||
      CopyBuffer(handleZigzag, 0, 0, 100, ZIGZAGBuffer)<=0||
      CopyBuffer(handleZigzag, 1, 0, 100, ZigzagHighBuffer)<=0||CopyBuffer(handleZigzag, 2, 0, 100, ZigzagLowBuffer)<=0||CopyBuffer(handleAMA, 0, 0, 10, AmaBuffer)<=0||
      CopyBuffer(handleTEMA, 0, 0, 10, TEMABuffer)<=0 || CopyBuffer(handleTEMA15, 0, 0, 10, TEMABuffer15)<=0 || CopyBuffer(handleTEMA5, 0, 0, 10, TEMABuffer5)<=0||
      CopyBuffer(handleADX, 0, 0, 10, AdxBuffer)<=0||CopyBuffer(handleADX, 1, 0, 10, AdxPDIBuffer)<=0||CopyBuffer(handleADX, 2, 0, 10, AdxNDIBuffer)<=0||
      CopyBuffer(handleADX5, 0, 0, 10, AdxBuffer5)<=0||CopyBuffer(handleADX5, 1, 0, 10, AdxPDIBuffer5)<=0||CopyBuffer(handleADX5, 2, 0, 10, AdxNDIBuffer5)<=0||
      CopyBuffer(handleRSI, 0, 0, 10, RsiBuffer)<=0||CopyBuffer(handleStdDev, 0, 0, 10, StdDevBuffer)<=0||CopyBuffer(handleAMA5, 0, 0, 10, AmaBuffer5)<=0||
      CopyBuffer(handleAMA15, 0, 0, 10, AmaBuffer15)<=0||CopyBuffer(handleAtr, 0, 0, 10, AtrBuffer)<=0||CopyBuffer(handleStdDev15, 0, 0, 10, StdDevBuffer15)<=0||
      CopyBuffer(handleAtr15, 0, 0, 10, AtrBuffer15)<=0||CopyBuffer(handleBollingerBand, 1, 0, 10, BBUpperBuffer)<=0||CopyBuffer(handleBollingerBand, 2, 0, 10, BBLowerBuffer)<=0||
      CopyBuffer(handleBollingerBand, 0, 0, 10, BBMiddleBuffer)<=0)
      return false;
   return true;
}

void CloseAllPositions()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            bool ret = trade.PositionClose(g_position.Ticket());
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseAllProfitable()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            if(g_position.Profit()>0)
            {
               bool ret = trade.PositionClose(g_position.Ticket());
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseAllNonProfitable()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            if(g_position.Profit()<=0)
            {
               bool ret = trade.PositionClose(g_position.Ticket());
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseOneSellPosition()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      if(autoTradedTickets.m_nMatchedSignalsCount>0)
      {
         int short_position = -1;
         for(int i = 0; i < autoTradedTickets.m_nMatchedSignalsCount; i++)
         {
            if(autoTradedTickets.m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind && autoTradedTickets.m_parr_matchedSignals[i].m_kind==SellSignalPattern)
            {
               short_position = i; break;
            }
         }
         if(short_position != -1)
         {
            trade.PositionClose(autoTradedTickets.m_parr_matchedSignals[short_position].m_ticketNumber);
         }
         else
         {
            int buy_position = -1;
            for(int i = 0; i < autoTradedTickets.m_nMatchedSignalsCount; i++)
            {
               if(autoTradedTickets.m_parr_matchedSignals[i].m_kind==SellSignalPattern)
                  buy_position = i;
            }
            if(buy_position!=-1)
               trade.PositionClose(autoTradedTickets.m_parr_matchedSignals[buy_position].m_ticketNumber);
         }
      }
      else
      {
         int buy_position = -1;
         ulong ticket = 0;
         for(int i = 0; i < position_cnt; i++)
         {
            if(g_position.SelectByIndex(i))
            {
               ticket = g_position.Ticket();
               if(g_position.PositionType()==POSITION_TYPE_SELL)
               {
                  buy_position = i;
                  break;
               }
            }
         }
         if(buy_position!=-1 && ticket!=0)
         {
            g_position.SelectByTicket(ticket);
            trade.PositionClose(ticket);
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseOneBuyPosition()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      if(autoTradedTickets.m_nMatchedSignalsCount>0)
      {
         int short_position = -1;
         for(int i = 0; i < autoTradedTickets.m_nMatchedSignalsCount; i++)
         {
            if(autoTradedTickets.m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind && autoTradedTickets.m_parr_matchedSignals[i].m_kind==BuySignalPattern)
            {
               short_position = i; break;
            }
         }
         if(short_position != -1)
         {
            trade.PositionClose(autoTradedTickets.m_parr_matchedSignals[short_position].m_ticketNumber);
         }
         else
         {
            int buy_position = -1;
            for(int i = 0; i < autoTradedTickets.m_nMatchedSignalsCount; i++)
            {
               if(autoTradedTickets.m_parr_matchedSignals[i].m_kind==BuySignalPattern)
                  buy_position = i;
            }
            if(buy_position!=-1)
               trade.PositionClose(autoTradedTickets.m_parr_matchedSignals[buy_position].m_ticketNumber);
         }
      }
      else
      {
         int buy_position = -1;
         ulong ticket = 0;
         for(int i = 0; i < position_cnt; i++)
         {
            if(g_position.SelectByIndex(i))
            {
               ticket = g_position.Ticket();
               if(g_position.PositionType()==POSITION_TYPE_BUY)
               {
                  buy_position = i;
                  break;
               }
            }
         }
         if(buy_position!=-1 && ticket!=0)
         {
            g_position.SelectByTicket(ticket);
            trade.PositionClose(ticket);
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseSellPositions(bool bProfit=false)
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            ulong ticket = g_position.Ticket();
            int matched_id = autoTradedTickets.findMatchedSignal(ticket);
            if(matched_id!=-1)
            {
               MatchedSignal* matchedSignal = autoTradedTickets.m_parr_matchedSignals[matched_id];
               if(matchedSignal.m_close_kind==LongTermSpecialCloseLongKind)
                  continue;
            }
            if(g_position.PositionType()==POSITION_TYPE_SELL)
            {
               if(bProfit==false||(bProfit&&g_position.Profit()>0))
               {
                  Print("close sell positions");
                  trade.PositionClose(g_position.Ticket());
               }
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseBuyPositions(bool bProfit=false)
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            ulong ticket = g_position.Ticket();
            int matched_id = autoTradedTickets.findMatchedSignal(ticket);
            if(matched_id!=-1)
            {
               MatchedSignal* matchedSignal = autoTradedTickets.m_parr_matchedSignals[matched_id];
               if(matchedSignal.m_close_kind==LongTermSpecialCloseLongKind)
               {
                  continue;
               }
            }

            if(g_position.PositionType()==POSITION_TYPE_BUY)
            {
               if(bProfit==false||(bProfit&&g_position.Profit()>0))
               {
                  Print("close buy positions");
                  trade.PositionClose(g_position.Ticket());
               }
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseSellPositions15()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            int matched_id = autoTradedTickets.findMatchedSignal(g_position.Ticket());
            if(matched_id!=-1)
            {
               MatchedSignal* matchedSignal = autoTradedTickets.m_parr_matchedSignals[matched_id];
               if(matchedSignal && (matchedSignal.m_close_kind==Macd15CloseKind))
               {
                  if(g_position.PositionType()==POSITION_TYPE_SELL)
                  {
                     Print("close sell positions 15");
                     trade.PositionClose(g_position.Ticket());
                  }
               }
            }
            
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseBuyPositions15()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            int matched_id = autoTradedTickets.findMatchedSignal(g_position.Ticket());
            if(matched_id!=-1)
            {
               MatchedSignal* matchedSignal = autoTradedTickets.m_parr_matchedSignals[matched_id];
               if(matchedSignal && (matchedSignal.m_close_kind==Macd15CloseKind))
               {
                  if(g_position.PositionType()==POSITION_TYPE_BUY)
                  {
                     Print("close buy positions 15");
                     trade.PositionClose(g_position.Ticket());
                  }
               }
            }
            
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseLostSellPosition()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            if(g_position.PositionType()==POSITION_TYPE_SELL)
            {
               if(g_position.Profit()<0)
                  trade.PositionClose(g_position.Ticket());
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void CloseLostBuyPosition()
{
   int position_cnt = PositionsTotal();  
   if(position_cnt > 0){
      for(int i = position_cnt-1; i >= 0; i--)
      {
         if(g_position.SelectByIndex(i))
         {
            if(g_position.PositionType()==POSITION_TYPE_BUY)
            {
               if(g_position.Profit()<0)
                  trade.PositionClose(g_position.Ticket());
            }
         }
      }
      autoTradedTickets.recalcMatchedSignals();
   }
}

void BuyAndCloseSellPositions()
{
   double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
   double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
   double lot = (specialTrading)?specialLots:myLots;
   double stop = (handBidAutoClose)?ask-stopLossHandDefault:0;//AtrBuffer[0]*3
   if(atrStoploss)
      stop = (handBidAutoClose)?ask-AtrBuffer[0]*2.5:0;//
   double tp = (handBidAutoClose)?ask+takeProfitDefault:0;
   bool ret = trade.Buy(lot, _symbol, ask, stop, tp);
   if(ret)
   {
      CloseKind closeKind = longTemaTrend?LongTermSpecialCloseShortKind:ZigzagCloseKind;
      autoTradedTickets.addMatchedSignal(BuySignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
   }
   CloseSellPositions(false);
}

void SellAndCloseBuyPositions()
{
   double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
   double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
   double lot = (specialTrading)?specialLots:myLots;
   double stop = (handBidAutoClose)?bid+stopLossHandDefault:0;//AtrBuffer[0]*3
   if(atrStoploss)
      stop = (handBidAutoClose)?bid+AtrBuffer[0]*2.5:0;//
   double tp = (handBidAutoClose)?bid-takeProfitDefault:0;
   bool ret = trade.Sell(lot, _symbol, bid, stop, tp);
   if(ret)
   {
      CloseKind closeKind = longTemaTrend?LongTermSpecialCloseShortKind:ZigzagCloseKind;
      autoTradedTickets.addMatchedSignal(SellSignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
   }
   CloseBuyPositions(false);
}

void BuyOnePosition(CloseKind closeKind=LongTermSpecialCloseShortKind)
{
   int res = MessageBox("100% 담보합니까?", "매수확인", MB_YESNO);
   if(res==IDYES)
   {
      if((_symbol=="BTCUSD"&&g_bid>resistance_price2-80)||(_symbol=="ETHUSD"&&g_bid>resistance_price2-5))
      {
         res = MessageBox("저항선근방입니다. 정말 매수하겠습니까?", "확인", MB_YESNO);
      }
   }
   if(((temaTrend==TrendUp)||(_symbol=="BTCUSD"&&g_bid<support_price2+80)||(_symbol=="ETHUSD"&&g_bid<support_price2+5)) && res==IDYES)
   {
      double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
      double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
      double lot = (specialTrading)?specialLots:myLots;
      double stop = (handBidAutoClose)?ask-stopLossHandDefault:0;//AtrBuffer[0]*3
      if(atrStoploss)
         stop = (handBidAutoClose)?ask-AtrBuffer[0]*2:0;//
      if(g_bid<support_price2+80)
         stop = (handBidAutoClose)?support_price2-20:0;
      double tp = (handBidAutoClose)?ask+takeProfitDefault:0;
      bool ret = trade.Buy(lot, _symbol, ask, stop, tp);
      if(ret)
      {
         if(closeKind==LongTermSpecialCloseShortKind||closeKind==LongTermSpecialCloseLongKind)
            autoTradedTickets.m_LongSpecialCount++;
         autoTradedTickets.addMatchedSignal(BuySignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
      }
   }
}

void SellOnePosition(CloseKind closeKind=LongTermSpecialCloseShortKind)
{
   int res = MessageBox("100% 담보합니까?", "매도확인", MB_YESNO);
   if(res==IDYES)
   {
      if((_symbol=="BTCUSD"&&g_bid<support_price2+80)||(_symbol=="ETHUSD"&&g_bid<support_price2+5))
      {
         res = MessageBox("지지선근방입니다. 정말 매도하겠습니까?", "확인", MB_YESNO);
      }
   }
   if(((temaTrend==TrendDown)||(_symbol=="BTCUSD"&&g_bid>resistance_price2-80)||(_symbol=="ETHUSD"&&g_bid>resistance_price2-5)) && res==IDYES)
   {
      double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
      double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
      double lot = (specialTrading)?specialLots:myLots;
      double stop = (handBidAutoClose)?bid+stopLossHandDefault:0;//AtrBuffer[0]*3
      if(atrStoploss)
         stop = (handBidAutoClose)?bid+AtrBuffer[0]*2:0;//
      if(g_bid>resistance_price2-80)
         stop = (handBidAutoClose)?resistance_price2+20:0;
      double tp = (handBidAutoClose)?bid-takeProfitDefault:0;
      bool ret = trade.Sell(lot, _symbol, bid, stop, tp);
      if(ret)
      {
         if(closeKind==LongTermSpecialCloseShortKind||closeKind==LongTermSpecialCloseLongKind)
            autoTradedTickets.m_LongSpecialCount++;
         autoTradedTickets.addMatchedSignal(SellSignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
      }
   }
}

void BuyOneSpecialPosition(bool bShort=true)
{
   int res = MessageBox("100% 담보합니까? 손실나면 제때에 닫으시오", "매수확인", MB_YESNO);
   if(res==IDYES)
   {
      if((_symbol=="BTCUSD"&&g_bid>resistance_price2-80)||(_symbol=="ETHUSD"&&g_bid>resistance_price2-5))
      {
         res = MessageBox("저항선근방입니다. 정말 매수하겠습니까?", "확인", MB_YESNO);
      }
   }
   if(((temaTrend==TrendUp)||(_symbol=="BTCUSD"&&g_bid<support_price2+80)||(_symbol=="ETHUSD"&&g_bid<support_price2+5)) && res==IDYES)
   {
      double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
      double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
      double lot = specialLots;
      double stop = (handBidAutoClose)?ask-stopLossHandDefault:0;//AtrBuffer[0]*3
      if(atrStoploss)
         stop = (handBidAutoClose)?ask-AtrBuffer[0]*2:0;//
      if(g_bid<support_price2+80)
         stop = (handBidAutoClose)?support_price2-20:0;
      double tp = (handBidAutoClose)?ask+takeProfitDefault:0;
      bool ret = trade.Buy(lot, _symbol, ask, stop, tp);
      if(ret)
      {
         CloseKind closeKind = (bShort)?LongTermSpecialCloseShortKind:LongTermSpecialCloseLongKind;
         autoTradedTickets.addMatchedSignal(BuySignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
      }
   }
}

void SellOneSpecialPosition(bool bShort=true)
{
   int res = MessageBox("100% 담보합니까? 손실나면 제때에 닫으시오", "매도확인", MB_YESNO);
   if(res==IDYES)
   {
      if((_symbol=="BTCUSD"&&g_bid<support_price2+80)||(_symbol=="ETHUSD"&&g_bid<support_price2+5))
      {
         res = MessageBox("지지선근방입니다. 정말 매도하겠습니까?", "확인", MB_YESNO);
      }
   }
   if(((temaTrend==TrendDown)||(_symbol=="BTCUSD"&&g_bid>resistance_price2-80)||(_symbol=="ETHUSD"&&g_bid>resistance_price2-5)) && res==IDYES)
   {
      double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
      double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
      double lot = specialLots;
      double stop = (handBidAutoClose)?bid+stopLossHandDefault:0;//AtrBuffer[0]*3
      if(atrStoploss)
         stop = (handBidAutoClose)?bid+AtrBuffer[0]*2:0;//
      if(g_bid>resistance_price2-80)
         stop = (handBidAutoClose)?resistance_price2+20:0;
      double tp = (handBidAutoClose)?bid-takeProfitDefault:0;
      bool ret = trade.Sell(lot, _symbol, bid, stop, tp);
      if(ret)
      {
         CloseKind closeKind = (bShort)?LongTermSpecialCloseShortKind:LongTermSpecialCloseLongKind;
         autoTradedTickets.addMatchedSignal(SellSignalPattern, ask, bid, defaultAfterBarCountToCheck, closeKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
      }
   }
}


bool CloseTicket(ulong ticketNumber)
{
   bool ret = trade.PositionClose(ticketNumber);
   if(ret)
   {
      autoTradedTickets.deleteMatchedSignalByTicket(ticketNumber);
   }
   return ret;
}

TrendPattern CheckLongTemaTrend() 
{
   if((TEMABuffer15[0]-TEMABuffer15[1]>temaThreshold) && (TEMABuffer15[1]-TEMABuffer15[2]>temaThreshold) && (TEMABuffer15[2]-TEMABuffer15[3]>temaThreshold) && (TEMABuffer15[3]-TEMABuffer15[4]>temaThreshold))
   {
      return TrendUp; 
   }
   else if((TEMABuffer15[1]-TEMABuffer15[0]>temaThreshold) && (TEMABuffer15[2]-TEMABuffer15[1]>temaThreshold) && (TEMABuffer15[3]-TEMABuffer15[2]>temaThreshold) && (TEMABuffer15[4]-TEMABuffer15[3]>temaThreshold))
   {
      return TrendDown;
   }
   return NoTrend;
}

TrendPattern CheckRsiTrend()
{
   if(RsiBuffer[0]>RsiBuffer[1] && RsiBuffer[0]>60)
      return TrendUp;
   else if(RsiBuffer[0]<RsiBuffer[1] && RsiBuffer[0]<40)
      return TrendDown;
   return NoTrend;
}

TrendPattern CheckAdxTrend()
{
   if(AdxBuffer[0]>AdxBuffer[1] && AdxBuffer[1]>AdxBuffer[2] && temaTrend==TrendUp)
      return TrendUp;
   else if(AdxBuffer[0]>AdxBuffer[1] && AdxBuffer[1]>AdxBuffer[2] && temaTrend==TrendDown)
      return TrendDown;
   return NoTrend;
}

TrendPattern CheckVolumeTrend()
{
   long volume_1 = iVolume(_symbol, PERIOD_CURRENT, 1);
   long volume_2 = iVolume(_symbol, PERIOD_CURRENT, 2);
   double open_1 = iOpen(_symbol, PERIOD_CURRENT, 1);
   double open_2 = iOpen(_symbol, PERIOD_CURRENT, 2);
   double close_1 = iClose(_symbol, PERIOD_CURRENT, 1);
   double close_2 = iClose(_symbol, PERIOD_CURRENT, 2);
   if(volume_1>volume_2)
   {
      if(open_1>close_1 && (open_1-close_1)>(open_2-close_2))
         return TrendDown;
      else if(open_1<close_1 && (close_1-open_1)>(close_2-open_2))
         return TrendUp;
   }
   return NoTrend;
}

TrendPattern CheckTEMATrend() 
{
   if(TEMABuffer[0]-TEMABuffer[1]>temaThreshold)
      return TrendUp; 
   else if(TEMABuffer[1]-TEMABuffer[0]>temaThreshold)
      return TrendDown;
   else
      return NoTrend;
}

TrendPattern CheckTEMA15Trend() 
{
   if(TEMABuffer15[0]-TEMABuffer15[1]>temaThreshold)
      return TrendUp; 
   else if(TEMABuffer15[1]-TEMABuffer15[0]>temaThreshold)
      return TrendDown;
   else
      return NoTrend;
}

TrendPattern CheckTEMA5Trend() 
{
   if(TEMABuffer5[0]-TEMABuffer5[1]>temaThreshold)
      return TrendUp; 
   else if(TEMABuffer5[1]-TEMABuffer5[0]>temaThreshold)
      return TrendDown;
   else
      return NoTrend;
}

SignalPattern CheckTEMASignal()
{
   if((TEMABuffer[0]-TEMABuffer[1])>0 && (TEMABuffer[1]-TEMABuffer[2])<0)
      return BuySignalPattern;
   else if((TEMABuffer[0]-TEMABuffer[1])<0 && (TEMABuffer[1]-TEMABuffer[2])>0)
      return SellSignalPattern;
   return NoSignalPattern;
}

SignalPattern CheckMACDSignal()
{
   if((MACDBuffer[0]-MACDBuffer[1])>0 && (MACDBuffer[1]-MACDBuffer[2])<0)
      return BuySignalPattern;
   else if((MACDBuffer[0]-MACDBuffer[1])<0 && (MACDBuffer[1]-MACDBuffer[2])>0)
      return SellSignalPattern;
   else if(MACDBuffer[0]>-macdThreshold && MACDBuffer[0]>MACDBuffer[1])
      return BuySignalPattern;
   else if(MACDBuffer[0]<macdThreshold && MACDBuffer[0]<MACDBuffer[1])
      return SellSignalPattern;
   return NoSignalPattern;
}

class PreMatchedSignalPattern
{
public:
   SignalPattern kind;//BuySignalPattern: buy, SellSignalPattern: sell, 0: no
   double order_price;
   datetime matchBarTime;
   int bar_count;
   CloseKind close_kind;
   PreMatchedSignalPattern(){
      kind = NoSignalPattern;
      order_price = 0;
      matchBarTime = 0;
      bar_count = 0;
      close_kind = MacdCloseKind;
   }
   void setValues(SignalPattern signal, double price, datetime bar_time, CloseKind closeKind){
      kind = signal;
      order_price = price;
      matchBarTime = bar_time;
      close_kind = closeKind;
   }
   void setBarCount(datetime bar_time){
      if(close_kind==MacdCloseKind)
         bar_count = (int)(double(bar_time-matchBarTime)/(60*15));
   }
   void reset(){
      kind = NoSignalPattern;
      order_price = 0;
      matchBarTime = 0;
      bar_count = 0;
      close_kind = MacdCloseKind;
   }
};

class MatchedSignal
{
public:
   SignalPattern m_kind; //BuySignalPattern: buy, SellSignalPattern: sell, 0: no
   double m_ask;
   double m_bid;
   double m_stop;
   int m_afterBarCountToCheck; //if followed bars doesn't match trend, close order
   CloseKind m_close_kind; //if MacdCloseKind: macd determined, ZigzagCloseKind: zigzag determined
   ulong m_ticketNumber; //ticket number
   datetime m_time;
   double m_lot;
   double m_target_profit;
   double m_max;
   bool m_stoploss_changed;
   MatchedSignal(SignalPattern kind, double ask, double bid, int afterBarCountToCheck, CloseKind close_kind, ulong ticketNumber, double lot, double target_profit){
      m_kind = kind;
      m_ask = ask;
      m_bid = bid;
      m_stop = 0;
      m_lot = lot;
      m_max = bid;
      m_stoploss_changed = false;
      m_afterBarCountToCheck = afterBarCountToCheck;
      m_close_kind = close_kind;
      m_ticketNumber = ticketNumber;
      m_time = iTime(_symbol, PERIOD_CURRENT, 0);
      m_target_profit = target_profit;
   }
};

class ChartAnalyzer
{
public:
   ChartAnalyzer();
   ~ChartAnalyzer();
   VolatilityType checkPrevVolatility();
   VolatilityType checkCurrVolatility();
   void setBoundLine();
   CandlePattern checkPrevCandleType();
   CandlePattern checkCandleType(double open, double high, double close, double low);
   int predictChart();
   void displayAnalyzeResult();
   bool DrawStraightLine(string name, datetime time1, double price1, color lineColor, ENUM_LINE_STYLE style, int width);
   TrendPattern candleTrend;
   double price_up;
   double price_down;
   double average_down;
   double average_up;
   double price_diff[11];
   int up_count;
   int down_count;
   VolatilityType prev_volatility;
   VolatilityType curr_volatility;
   void addToPriceDiff(double diff);
   TrendPattern checkPriceDiffTrend();
};

ChartAnalyzer::ChartAnalyzer()
{
   candleTrend = NoTrend;
   prev_volatility = Stable;
   curr_volatility = Stable;
   price_up = 0;
   price_down = 0;
   up_count = 0;
   down_count = 0;
   average_down = 0;
   average_up = 0;
   
}
ChartAnalyzer::~ChartAnalyzer()
{
}
TrendPattern ChartAnalyzer::checkPriceDiffTrend(void)
{
   int check_count = 8;
   int up_cnt = 0;
   int down_cnt = 0;
   if(check_count<10)
   for(int i = 10-check_count-1; i < 9; i++)
   {
      if(price_diff[i+1]>price_diff[i])
         up_cnt++;
      else if(price_diff[i+1]<price_diff[i])
         down_cnt++;
   }
   if(up_cnt==check_count)
      return TrendUp;
   else if(down_cnt==check_count)
      return TrendDown;
   return NoTrend;
}
void ChartAnalyzer::addToPriceDiff(double diff)
{
   for(int i = 0; i < 9; i++)
   {
      price_diff[i] = price_diff[i+1];
   }
   price_diff[9] = diff;
}

CandlePattern ChartAnalyzer::checkCandleType(double open,double high,double close,double low)
{
   double body_len = MathAbs(open-close);
   double total_len = high-low;
   double lower_shadow = MathMin(open, close)-low;
   double upper_shadow = high-MathMax(open, close);
   if(lower_shadow<body_len*0.1 && upper_shadow<body_len*0.1)
   {
      if(open<close)
         return LongMarubozu;
      if(close<open)
         return ShortMarubozu;
   }
   if(body_len<20 && lower_shadow<body_len*0.5 && upper_shadow<body_len*0.5)
   {
      return SpinningTop;
   }
   if(lower_shadow>body_len*2 && upper_shadow<body_len*0.1)
      return BullishHammer;
   if(lower_shadow<body_len*0.1 && upper_shadow>body_len*2)
      return BearishHammer;
   return Unknown;
}
void ChartAnalyzer::displayAnalyzeResult(void)
{
   setBoundLine();
   prev_volatility = checkPrevVolatility();
   curr_volatility = checkCurrVolatility();
   CandlePattern prev_candle = checkPrevCandleType();
   string title = "Analyze:";
   string prev_volatility_str = (prev_volatility==Large)?"불안정":(prev_volatility==Small)?"준안정":"안정";
   string curr_volatility_str = (curr_volatility==Large)?"불안정":(curr_volatility==Small)?"준안정":"안정";;
   string prev_candle_str = (prev_candle==LongMarubozu)?"LongMarubozu":(prev_candle==ShortMarubozu)?"ShortMarubozu":(prev_candle==SpinningTop)?"SpinningTop":
                  (prev_candle==BullishHammer)?"BullishHammer":(prev_candle==BearishHammer)?"BearishHammer":"Unknown";
   string zig_trend_str = (zigzagTrend==TrendUp)?"상승":(zigzagTrend==TrendDown)?"하락":"없음";
   string curr_trend_str = (candleTrend==TrendUp)?"상승":(candleTrend==TrendDown)?"하락":"없음";
   string potential_trend_str = (potentialTrend==PotentialTrendUp)?"상승예고":(potentialTrend==PotentialTrendDown)?"하락예고":"없음";
   string buy_sell_state_str = (RsiBuffer[0]>70)?"과매수":(RsiBuffer[0]<30)?"과매도":"보통";
   double profit = 0;
   if(autoTradedTickets.m_nMatchedSignalsCount>0)
   {
      g_position.SelectByTicket(autoTradedTickets.m_parr_matchedSignals[0].m_ticketNumber);
      profit = g_position.Profit();
   }
   string label_text1 = StringFormat("이전 변동성: %s, 현캔들 변동성: %s, 상승: %d, 하락: %d", prev_volatility_str, curr_volatility_str, up_count, down_count);
   string label_text2 = StringFormat("zig추세: %s, 캔들추세: %s, 가격차: %.2f, 리윤: %.2f", zig_trend_str, curr_trend_str, price_up-price_down, profit);
   string label_text3 = StringFormat("매매상태: %s, 상승폭:%.2f, 하락폭:%.2f, 이전폭: %.2f, 현재폭: %.2f", buy_sell_state_str, price_up, price_down, prev_average_tick_volume, recent_average_tick_volume);
   addToPriceDiff(price_up-price_down);
   ObjectSetString(0, ANALYZE_LABEL1, OBJPROP_TEXT, label_text1);
   ObjectSetString(0, ANALYZE_LABEL2, OBJPROP_TEXT, label_text2);
   ObjectSetString(0, ANALYZE_LABEL3, OBJPROP_TEXT, label_text3);
   if(price_diff[9]>=price_diff[8])
   {
     ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_COLOR, clrGreenYellow);
     ObjectSetString(0, ARROW_UP_DOWN, OBJPROP_TEXT, CharToString(233));
   }
   else if(price_diff[9]<=price_diff[8])
   {
     ObjectSetInteger(0, ARROW_UP_DOWN, OBJPROP_COLOR, clrRed);
     ObjectSetString(0, ARROW_UP_DOWN, OBJPROP_TEXT, CharToString(234));
   }
//   Comment(label_text);
}
VolatilityType ChartAnalyzer::checkPrevVolatility(void)
{
   CandlePattern prev_candles[4];
   double total_tail_rate = 0;
   double tail_rate = 0;
   for(int i = 1; i < 5; i++)
   {
      double open = iOpen(_symbol, PERIOD_M1, i);
      double close = iClose(_symbol, PERIOD_M1, i);
      double high = iHigh(_symbol, PERIOD_M1, i);
      double low = iLow(_symbol, PERIOD_M1, i);
      prev_candles[i-1] = checkCandleType(open, high, close, low);
      double tail_len = (open>close)?(high-open)+(close-low):(high-close)+(open-low);
      tail_rate = tail_len/(high-low);
      total_tail_rate += tail_rate;
   }
   double mean_tail_rate = total_tail_rate/5;
   if(prev_candles[0]==LongMarubozu && prev_candles[1]==LongMarubozu)
      return Stable;
   else if(prev_candles[0]==ShortMarubozu && prev_candles[1]==ShortMarubozu)
      return Stable;
   else if((prev_candles[0]==LongMarubozu&&prev_candles[1]==ShortMarubozu)||(prev_candles[0]==ShortMarubozu&&prev_candles[1]==LongMarubozu))
      return Large;
   else if((prev_candles[0]==BullishHammer||prev_candles[0]==BearishHammer)&&(prev_candles[1]!=ShortMarubozu&&prev_candles[1]!=LongMarubozu))
      return Large;
   else if(mean_tail_rate<0.1)
      return Stable;
   else if(mean_tail_rate<0.5)
      return Small;
   return Large;
}
VolatilityType ChartAnalyzer::checkCurrVolatility(void)
{
   up_count = 0;
   down_count = 0;
   price_up = 0;
   price_down = 0;
   average_down = 0;
   average_up = 0;
   int average_down_count = 0;
   int average_up_count = 0;
   for(int i = 1; i < autoTradedTickets.m_ten_prices_count; i++)
   {
      if(autoTradedTickets.m_ten_prices[i-1]<autoTradedTickets.m_ten_prices[i])
      {
         up_count++;
         price_up += (autoTradedTickets.m_ten_prices[i]-autoTradedTickets.m_ten_prices[i-1]);
         if(i>1 && autoTradedTickets.m_ten_prices[i-2]>autoTradedTickets.m_ten_prices[i-1])
         {
            average_up_count++;
            average_up += (autoTradedTickets.m_ten_prices[i]-autoTradedTickets.m_ten_prices[i-1]);
         }
      }
      else if(autoTradedTickets.m_ten_prices[i-1]>autoTradedTickets.m_ten_prices[i])
      {
         down_count++;
         price_down += (autoTradedTickets.m_ten_prices[i-1]-autoTradedTickets.m_ten_prices[i]);
         if(i>1 && autoTradedTickets.m_ten_prices[i-2]<autoTradedTickets.m_ten_prices[i-1])
         {
            average_down_count++;
            average_down += (autoTradedTickets.m_ten_prices[i-1]-autoTradedTickets.m_ten_prices[i]);
         }
      }
   }
   average_down = average_down/average_down_count;
   average_up = average_up/average_up_count;
   if(up_count>down_count && up_count>autoTradedTickets.m_ten_prices_count*0.7 && price_up>price_down*2)
   {
      candleTrend = TrendUp;
      return Stable;
   }
   if(down_count>up_count && down_count>autoTradedTickets.m_ten_prices_count*0.7 && price_down>price_up*2)
   {
      candleTrend = TrendDown;
      return Stable;
   }
   if(up_count>down_count && up_count>autoTradedTickets.m_ten_prices_count*0.6 && price_up>price_down)
   {
      candleTrend = TrendUp;
      return Small;
   }
   if(down_count>up_count && down_count>autoTradedTickets.m_ten_prices_count*0.6 && price_down>price_up)
   {
      candleTrend = TrendDown;
      return Small;
   }
   if(price_down>price_up)
      candleTrend = TrendDown;
   if(price_down<price_up)
      candleTrend = TrendUp;
   return Large;
}
void ChartAnalyzer::setBoundLine(void)
{
   resistance_time = 0;
   support_time = 0;
   for(int i = 0; i < 100; i++)
   {
      if(ZIGZAGBuffer[i]!=0 && ZigzagHighBuffer[i]!=0 && (resistance_price<ZigzagHighBuffer[i]||resistance_time==0))
      {
         resistance_price = ZigzagHighBuffer[i];
         resistance_time = iTime(_symbol, PERIOD_M1, i);
         if(i<50)
         {
            resistance_price2 = resistance_price;
            resistance_time2 = resistance_time;
         }
      }
      else if(ZIGZAGBuffer[i]!=0 && ZigzagLowBuffer[i]!=0 && (support_price>ZigzagLowBuffer[i]||support_time==0))
      {
         support_price = ZigzagLowBuffer[i];
         support_time = iTime(_symbol, PERIOD_M1, i);
         if(i<50)
         {
            support_price2 = support_price;
            support_time2 = support_time;
         }
      }
   }
   string resi_name = "Resistance";
   DrawStraightLine(resi_name, resistance_time, resistance_price, clrWhite);
   string supp_name = "Support";
   DrawStraightLine(supp_name, support_time, support_price, clrYellowGreen);
   string resi_name2 = "Resistance2";
   DrawStraightLine(resi_name2, resistance_time, resistance_price2, clrYellow);
   string supp_name2 = "Support2";
   DrawStraightLine(supp_name2, support_time, support_price2, clrOrange);
}
CandlePattern ChartAnalyzer::checkPrevCandleType(void)
{
   double open = iOpen(_symbol, PERIOD_M1, 1);
   double close = iClose(_symbol, PERIOD_M1, 1);
   double high = iHigh(_symbol, PERIOD_M1, 1);
   double low = iLow(_symbol, PERIOD_M1, 1);
   return checkCandleType(open, high, close, low);
}
int ChartAnalyzer::predictChart(void)
{
   return 0;
}
bool ChartAnalyzer::DrawStraightLine(string name, datetime time1, double price1, 
                     color lineColor=clrBlue, ENUM_LINE_STYLE style=STYLE_SOLID, int width=1)
{
   // 선이 이미 존재하는지 확인
   if(ObjectFind(0, name) != -1)
     {
      // 기존 선을 삭제
      ObjectDelete(0, name);
     }
   
   // Trend Line 객체 생성
   if(!ObjectCreate(0, name, OBJ_HLINE, 0, time1, price1))
     {
      Print("Failed to create trend line: ", name, " Error: ", GetLastError());
      return(false);
     }
   
   // 선의 속성 설정
   ObjectSetInteger(0, name, OBJPROP_COLOR, lineColor);
   ObjectSetInteger(0, name, OBJPROP_STYLE, style);
   ObjectSetInteger(0, name, OBJPROP_WIDTH, width);
   
   // 선 고정: false로 설정 시, 선을 차트 이동 시 함께 이동하지 않도록 설정
   ObjectSetInteger(0, name, OBJPROP_RAY_RIGHT, false);
   ObjectSetInteger(0, name, OBJPROP_RAY_LEFT, false);
   
   return(true);
}

class AutoTradedTickets
{
public:
   PreMatchedSignalPattern m_preMatchedPattern;

   MatchedSignal* m_parr_matchedSignals[];
   MatchedSignal* m_macdMatchedSignal;
   MatchedSignal* m_leapMatchedSignal;
   int m_nMatchedSignalsCount;
   double m_ten_prices[];
   int m_ten_prices_count;
   double m_high;
   double m_low;
   int m_LongSpecialCount;
   TrendPattern lastMatchedTrend;
   
public:
       AutoTradedTickets(void);
      ~AutoTradedTickets(void);
   SignalPattern getMacdSignalPattern();
   SignalPattern getMacd15SignalPattern();
   SignalPattern getLeapPattern();
   SignalPattern getStableSignalPattern();
   SignalPattern getAmaTemaSignalPattern();
   SignalPattern getAmaTema15SignalPattern();
   SignalPattern getMaxMinSignalPattern(double &zigzag[], double &high[], double &low[], int &highPoints[], int &lowPoints[]);
   void bidAgainConsistencyPattern();
   TrendPattern DetermineTrendByZigzag(double &zigzag[], double &high[], double &low[], int &highPoints[], int &lowPoints[]);
   TrendPattern DetermineTrend(double &zigzag[], double &high[], double &low[]);
   TrendPattern DetermineTrendByAMA(double &ama[]);
   TrendPattern DetectLongTermTrend();
   TrendPattern checkConsistency();
   bool checkStdAtrCondition();
   bool checkLeapCandle();
   bool checkAmatemaPatternProfit(datetime curr_time);
   bool checkBidAgainCondition();
   int countMatchInTrend(SignalPattern signal);
   int countMacd15MatchInTrend(SignalPattern signal);
   int countLeapMatchInTrend(SignalPattern signal);
   void addPriceToTenPrices(double bid);
   void CloseWrongPositions();
   void CloseOrderPositions(SignalPattern kind, bool bProfit);
   void CloseLongSpecialPosition();
   bool checkIfPreMatchedSignalPatternUpdatable(SignalPattern kind, double ask, double bid);
   bool checkIfPreMatchedSignalPatternExists();
   bool checkPreMatchedSignalPatternConsistency(double ask, double bid);
   SignalPattern getOrderSignalFromPreMatchedSignalPattern();
   int deleteMatchedSignalById(int id);
   int deleteMatchedSignalByTicket(ulong ticketNumber);
   MatchedSignal* getLastMatchedSignal(CloseKind kind, SignalPattern signalPattern);
   MatchedSignal* getLastSignal();
   int findMatchedSignal(ulong ticketNumber);
   double getMinMaxTargetPrice(SignalPattern signal);
   int addMatchedSignal(SignalPattern kind,double ask,double bid,int afterBarCountToCheck, CloseKind closeKind, ulong ticketNumber, double lot, double target_profit);
   void recalcMatchedSignals();
   double getCurrEquity();
   double getCurrBalance();
   double getFreeMargin();
   double getMargin();
   void closePositionsUsingStopLoss(void);
   void setStopLoss(void);
};

AutoTradedTickets::AutoTradedTickets(void):m_nMatchedSignalsCount(0)
{
   ArrayResize(m_parr_matchedSignals, 100);
   m_macdMatchedSignal = NULL;
   m_leapMatchedSignal = NULL;
   m_high = 0;
   m_low = DBL_MAX;
   m_LongSpecialCount = 0;
   m_ten_prices_count = 50;
   lastMatchedTrend = NoTrend;
   ArrayResize(m_ten_prices, m_ten_prices_count+1);
   ArrayInitialize(m_ten_prices, 0);
}

AutoTradedTickets::~AutoTradedTickets(void)
{
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      delete m_parr_matchedSignals[i];
   }
   
}

void AutoTradedTickets::setStopLoss(void)
{
   double profit = 0;
   datetime curr_time = TimeCurrent();
   curr_time = RemoveSeconds(curr_time);
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind!=LeapCloseKind)//curr_time > m_parr_matchedSignals[i].m_time && 
      {
         if(m_parr_matchedSignals[i].m_kind==BuySignalPattern)
         {
            if(g_bid > g_prev_bid)
               m_parr_matchedSignals[i].m_max = g_bid;
            profit = (g_bid-m_parr_matchedSignals[i].m_ask)*m_parr_matchedSignals[i].m_lot;//
            if(profit>m_parr_matchedSignals[i].m_lot*300 && m_parr_matchedSignals[i].m_stoploss_changed==false)
            {
               trade.PositionModify(m_parr_matchedSignals[i].m_ticketNumber, m_parr_matchedSignals[i].m_ask-10, PositionGetDouble(POSITION_TP));
               m_parr_matchedSignals[i].m_stoploss_changed = true;
            }
            if(m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind)
            {
               if(profit>m_parr_matchedSignals[i].m_lot*500&&g_prev_bid<g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, g_bid-profitBackStopLoss);//MathMin(AtrBuffer[0],50);
               }
/*               if(profit>m_parr_matchedSignals[i].m_lot*300&&g_prev_bid<g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, g_bid-profitBackStopLoss*0.7);//MathMin(AtrBuffer[0],50);
               }*/
            }
            else
            {
               if(profit>m_parr_matchedSignals[i].m_lot*300&&g_prev_bid<g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, g_bid-profitBackStopLoss);//MathMin(AtrBuffer[0],50);
               }
               else if(profit>m_parr_matchedSignals[i].m_lot*100&&g_prev_bid<g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, g_bid-profitBackStopLoss*0.7);//MathMin(AtrBuffer[0],50);
               }
               else if(profit>m_parr_matchedSignals[i].m_lot*50&&g_prev_bid<g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, g_bid-profitBackStopLoss*0.5);//MathMin(AtrBuffer[0],50);
               }
               else if(profit>0&&g_prev_bid<g_bid&&AtrBuffer[0]<80&&StdDevBuffer[0]<80)
               {
                  m_parr_matchedSignals[i].m_stop = MathMax(m_parr_matchedSignals[i].m_stop, m_parr_matchedSignals[i].m_bid);
               }
            }
         }
         else if(m_parr_matchedSignals[i].m_kind==SellSignalPattern)
         {
            if(g_bid<g_prev_bid)
               m_parr_matchedSignals[i].m_max = g_bid;
            profit = (m_parr_matchedSignals[i].m_bid-g_ask)*m_parr_matchedSignals[i].m_lot;
            if(profit>m_parr_matchedSignals[i].m_lot*300 && m_parr_matchedSignals[i].m_stoploss_changed==false)
            {
               trade.PositionModify(m_parr_matchedSignals[i].m_ticketNumber, m_parr_matchedSignals[i].m_bid+10, PositionGetDouble(POSITION_TP));
               m_parr_matchedSignals[i].m_stoploss_changed = true;
            }
            if(m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind)
            {
               if(profit>m_parr_matchedSignals[i].m_lot*500&&g_prev_bid>g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, g_bid+profitBackStopLoss):g_bid+profitBackStopLoss;//MathMin(AtrBuffer[0],50);
               }
/*               if(profit>m_parr_matchedSignals[i].m_lot*300&&g_prev_bid>g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, g_bid+profitBackStopLoss*0.7):g_bid+profitBackStopLoss*0.7;//MathMin(AtrBuffer[0],50);
               }*/
            }
            else
            {
               if(profit>m_parr_matchedSignals[i].m_lot*300&&g_prev_bid>g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, g_bid+profitBackStopLoss):g_bid+profitBackStopLoss;//MathMin(AtrBuffer[0],50);
               }
               if(profit>m_parr_matchedSignals[i].m_lot*100&&g_prev_bid>g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, g_bid+profitBackStopLoss*0.7):g_bid+profitBackStopLoss*0.7;//MathMin(AtrBuffer[0],50);
               }
               if(profit>m_parr_matchedSignals[i].m_lot*50&&g_prev_bid>g_bid)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, g_bid+profitBackStopLoss*0.5):g_bid+profitBackStopLoss*0.5;//MathMin(AtrBuffer[0],50);
               }
               if(profit>0&&g_prev_bid>g_bid&&AtrBuffer[0]<80&&StdDevBuffer[0]<80)
               {
                  m_parr_matchedSignals[i].m_stop = (m_parr_matchedSignals[i].m_stop>0)?MathMin(m_parr_matchedSignals[i].m_stop, m_parr_matchedSignals[i].m_bid):m_parr_matchedSignals[i].m_bid;
               }
            }
         }
      }
      else if(m_parr_matchedSignals[i].m_close_kind==LeapCloseKind)
      {
         if(m_parr_matchedSignals[i].m_kind==BuySignalPattern)
         {
            profit = (g_bid-m_parr_matchedSignals[i].m_ask)*m_parr_matchedSignals[i].m_lot;
            if(profit>m_parr_matchedSignals[i].m_lot*500&&g_prev_bid<g_bid)
            {
               m_parr_matchedSignals[i].m_stop = g_bid-profitBackStopLoss;
            }
         }
         else if(m_parr_matchedSignals[i].m_kind==SellSignalPattern)
         {
            profit = (m_parr_matchedSignals[i].m_bid-g_ask)*m_parr_matchedSignals[i].m_lot;
            if(profit>m_parr_matchedSignals[i].m_lot*500&&g_prev_bid>g_bid)
            {
               m_parr_matchedSignals[i].m_stop = g_bid+profitBackStopLoss;
            }
         }
      }
   }
}

void AutoTradedTickets::closePositionsUsingStopLoss(void)
{
   int i = 0;
   while(i < m_nMatchedSignalsCount)
   {
      if(m_parr_matchedSignals[i].m_stop!=0)
      {
         g_position.SelectByTicket(m_parr_matchedSignals[i].m_ticketNumber);
         if(g_position.Profit() > 0)
         {
            if(m_parr_matchedSignals[i].m_kind==BuySignalPattern)
            {
               if(g_bid<=m_parr_matchedSignals[i].m_stop)
               {
                  Print("closePositionsUsingStopLoss m_parr_matchedSignals[i].m_kind==BuySignalPattern");
                  bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }
/*               else if(m_parr_matchedSignals[i].m_stop==m_parr_matchedSignals[i].m_bid&&m_parr_matchedSignals[i].m_max-g_bid>20)
               {
                  Print("closePositionsUsingStopLoss buy stop lower");
                  bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }*/
            }
            else if(m_parr_matchedSignals[i].m_kind==SellSignalPattern)
            {
               if(g_bid>=m_parr_matchedSignals[i].m_stop)
               {
                  Print("closePositionsUsingStopLoss m_parr_matchedSignals[i].m_kind==SellSignalPattern");
                  bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }
/*               else if(m_parr_matchedSignals[i].m_stop==m_parr_matchedSignals[i].m_bid&&g_bid-m_parr_matchedSignals[i].m_max>20)
               {
                  Print("closePositionsUsingStopLoss sell stop lower");
                  bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }*/
            }
         }
      }
      i++;
   }   
}

int AutoTradedTickets::countMatchInTrend(SignalPattern signal)
{
   int count = 0;
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind==MacdCloseKind||m_parr_matchedSignals[i].m_close_kind==AmaTemaCloseKind||m_parr_matchedSignals[i].m_close_kind==MaxMinCloseKind)
      {
         if(m_parr_matchedSignals[i].m_kind==signal)
            count++;
      }
   }
   return count;
}

int AutoTradedTickets::countMacd15MatchInTrend(SignalPattern signal)
{
   int count = 0;
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind==Macd15CloseKind)
      {
         if(m_parr_matchedSignals[i].m_kind==signal)
            count++;
      }
   }
   return count;
}

int AutoTradedTickets::countLeapMatchInTrend(SignalPattern signal)
{
   int count = 0;
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind==LeapCloseKind)
      {
         if(m_parr_matchedSignals[i].m_kind==signal)
            count++;
      }
   }
   return count;
}

bool AutoTradedTickets::checkLeapCandle(void)
{
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind==LeapCloseKind)
      {
         if(m_parr_matchedSignals[i].m_time==currBarTime)
            return true;
      }
   }
   return false;
}

bool AutoTradedTickets::checkStdAtrCondition(void)
{
   bool StdDevCond = true;
   for(int i = 1; i < 2; i++)
   {
      StdDevCond &= (StdDevBuffer[0]>StdDevBuffer[i]);
   }
   bool AdxCond = true;
   for(int i = 1; i < 2; i++)
   {
      AdxCond &= (AdxBuffer[0]>AdxBuffer[i]);
   }
   bool StdAtrBidCond = (StdDevCond & AdxCond);
   return StdAtrBidCond;   
}

TrendPattern AutoTradedTickets::checkConsistency(void)
{
   int total_diff = 0;
   for(int i = m_ten_prices_count-10; i < m_ten_prices_count-1; i++)
   {
      int diff = 0;
      if(m_ten_prices[i+1]-m_ten_prices[i] < 0)
         diff = -1;
      else if(m_ten_prices[i+1]-m_ten_prices[i] > 0)
         diff = 1;
      total_diff += diff;
   }
   if(total_diff>=7)
      return TrendUp;
   else if(total_diff<=-7)
      return TrendDown;
   return NoTrend;
}

bool AutoTradedTickets::checkAmatemaPatternProfit(datetime curr_time)
{
//   if((MathAbs(AmaBuffer[0]-AmaBuffer[1])>50) && (MathAbs(AmaBuffer[0]-TEMABuffer15[0])>150))
//      return true;
   if(AdxBuffer[0]<30)
      return false;
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_close_kind==AmaTemaCloseKind)
      {
         int barCount = (int)(double(curr_time-m_parr_matchedSignals[i].m_time)/(60*15));
         if(((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&(AmaBuffer[0]-AmaBuffer[1])>50&&(g_bid-m_parr_matchedSignals[i].m_bid)>20)||
            (m_parr_matchedSignals[i].m_kind==SellSignalPattern&&(AmaBuffer[1]-AmaBuffer[0])>50&&(g_bid-m_parr_matchedSignals[i].m_bid)<-20)) && 
            (MathAbs(AmaBuffer[barCount]-TEMABuffer15[barCount])>amatemaDiffThreshold) && (MathAbs(AmaBuffer[0]-TEMABuffer15[0])>150))
            return true;
      }
   }
   return false;
}

bool AutoTradedTickets::checkBidAgainCondition(void)
{
   SignalPattern signal = amatemaSignal;
   int count = countMatchInTrend(signal);
   double tema = (m15)?TEMABuffer15[0]:TEMABuffer[0];
   if(MathAbs(AmaBuffer[0]-tema)<amatemaDiffThreshold)
      return false;
   if(signal==BuySignalPattern && temaTrend15==TrendUp && temaTrend==TrendUp && chartAnalyzer.candleTrend==TrendUp && count==0)
   {
      return true;
   }
   else if(signal==SellSignalPattern && temaTrend15==TrendDown && temaTrend==TrendDown && chartAnalyzer.candleTrend==TrendDown && count==0)
   {
      return true;
   }
   return false;
}

void AutoTradedTickets::bidAgainConsistencyPattern(void)
{
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   if(curr_bar_time!=consistencyClosedTime && consistencyClosedTime!=0)
   {
      consistencyClosedTime = 0;
      consistencyCloseLot = 0;
      return;
   }
   TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
   double ask = SymbolInfoDouble(_symbol, SYMBOL_ASK);
   double bid = SymbolInfoDouble(_symbol, SYMBOL_BID);
   double lot = consistencyCloseLot;
   int buy_count = countMatchInTrend(BuySignalPattern);
   int sell_count = countMatchInTrend(SellSignalPattern);
   if(temaTrend==TrendDown && consistency==TrendDown && sell_count<bidLimitInTrend)
   {
      double stop = (autoBidAutoClose)?bid+stopLossAutoDefault:0;
      double tp = (autoBidAutoClose)?bid-takeProfitDefault:0;
      bool ret = trade.Sell(lot, _symbol, bid, stop, tp);
      if(ret)
      {
         Print("consistency auto sell");
         autoTradedTickets.addMatchedSignal(SellSignalPattern, ask, bid, defaultAfterBarCountToCheck, LongTermSpecialCloseLongKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
         consistencyClosedTime = 0;
         consistencyCloseLot = 0;
      }
   }
   else if(temaTrend==TrendUp && consistency==TrendUp && buy_count<bidLimitInTrend)
   {
      double stop = (autoBidAutoClose)?ask-stopLossHandDefault:0;
      double tp = (autoBidAutoClose)?ask+takeProfitDefault:0;
      bool ret = trade.Buy(lot, _symbol, ask, stop, tp);
      if(ret)
      {
         Print("consistency auto buy");
         autoTradedTickets.addMatchedSignal(BuySignalPattern, ask, bid, defaultAfterBarCountToCheck, LongTermSpecialCloseLongKind, trade.ResultOrder(), lot, AtrBuffer[0]*3);
         consistencyClosedTime = 0;
         consistencyCloseLot = 0;
      }
   }
}

SignalPattern AutoTradedTickets::getAmaTemaSignalPattern(void)
{
   datetime curr_time_origin = TimeCurrent();
   datetime curr_time = RemoveSeconds(curr_time_origin);
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   double sec = double(curr_time_origin-curr_bar_time);

/*   if((AtrBuffer[0]<10 && StdDevBuffer[0]<10))//
      return NoSignalPattern;
   if(StdDevBuffer15[0]<100)
      return NoSignalPattern;
*/   
   bNoBidFor15Min = false;
   int sell_count = countMatchInTrend(SellSignalPattern);
   int buy_count = countMatchInTrend(BuySignalPattern);
   TrendPattern candleTrend = chartAnalyzer.candleTrend;
   double currTema = (m15)?TEMABuffer15[0]:TEMABuffer[0];
   double prevTema = (m15)?TEMABuffer15[1]:TEMABuffer[1];
   double pprevTema = (m15)?TEMABuffer15[2]:TEMABuffer[2];
   long volume2 = iVolume(_symbol, PERIOD_M1, 2);
   double body_len2 = iOpen(_symbol, PERIOD_M1, 2)-iClose(_symbol, PERIOD_M1, 2);
   long volume1 = iVolume(_symbol, PERIOD_M1, 1);
   double body_len1 = iOpen(_symbol, PERIOD_M1, 1)-iClose(_symbol, PERIOD_M1, 1);
   double open0 = iOpen(_symbol, PERIOD_M1, 0);
   double close0 = iClose(_symbol, PERIOD_M1, 0);
   if(((prevTema>AmaBuffer[1]&&currTema<AmaBuffer[0])||(prevTema<AmaBuffer[1]&&currTema>AmaBuffer[0]))&&sell_count==0&&buy_count==0&&curr_bar_time!=amatemaLastTick)//
   {
      g_bidCountInTrack = 0;
   }
   if(resistance_price2-support_price2<250)
   {
      bool amaHorizontalState = true;
      for(int i = 0; i < 6; i++)
      {
         if(AmaBuffer15[i]<resistance_price2&&AmaBuffer15[i]>support_price2&&MathAbs(AmaBuffer15[i]-AmaBuffer15[i+1])<7)
            amaHorizontalState &= true;
         else
            amaHorizontalState &= false;
      }
      if(amaHorizontalState)
         return NoSignalPattern;
   }
   int nbidLimitInTrack = 1;//(MathAbs(TEMABuffer15[0]-TEMABuffer15[1])>200)?bidLimitInTrack+1:bidLimitInTrack;
   if(chartAnalyzer.prev_volatility!=Large && !(MathAbs(support_price2-resistance_price2)<200 && (AmaBuffer15[0]<resistance_price2&&AmaBuffer15[0]>support_price2)))//
   {
         TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
         if(AmaBuffer[0]>currTema && AmaBuffer[0]>g_bid && body_len1>0 && ((AdxBuffer[1]>AdxBuffer[2]&&AdxBuffer[0]-AdxBuffer[1]>AdxBuffer[1]-AdxBuffer[2])||(TEMABuffer15[0]<TEMABuffer15[1]-200)))// && g_bidCountInTrack>0
         {
            if((temaTrend15==TrendDown && temaTrend==TrendDown && volumeTrend!=TrendUp && amaTrend==TrendDown))//
            {
               if(sell_count<bidLimitInTrend && g_bidCountInTrack<nbidLimitInTrack && AtrBuffer[0]>40 && (StdDevBuffer[0]>50||StdDevBuffer[0]>StdDevBuffer[1]) && 
                  AdxNDIBuffer[0]>AdxPDIBuffer[0] && AdxBuffer[0]>AdxBuffer[1] && zigzagTrend!=TrendUp && RsiBuffer[0]>30 && (g_bid>support_price2+50))
   //            if(sell_count<bidLimitInTrend && (g_bidCountInTrack<bidLimitInTrack&&StdAtrBidCond) && ((StdDevBuffer[0]>50)||RsiBuffer[0]>30) && zigzagTrend!=TrendUp && candleTrend==TrendDown && ((g_bid>support_price2+50&&AtrBuffer[0]<30)||AtrBuffer[0]>30))// && amaTrend!=TrendUp
               {
                  PrintFormat("StdAtrBidCond sell", g_bidCountInTrack);
                  if(amatemaTrading)
                  {
                     g_bidCountInTrend++;
                     g_bidCountInTrack++;
                  }
                  amatemaLastTick = curr_bar_time;
                  amatemaSignal = SellSignalPattern;
                  lastMatchedTrend = TrendDown;
                  return SellSignalPattern;
               }
            }
         }
         else if(AmaBuffer[0]<currTema && AmaBuffer[0]<g_bid && body_len1<0 && ((AdxBuffer[1]>AdxBuffer[2]&&AdxBuffer[0]-AdxBuffer[1]>AdxBuffer[1]-AdxBuffer[2])||(TEMABuffer15[0]>TEMABuffer15[1]+200)))// && g_bidCountInTrack>0
         {
            if((temaTrend15==TrendUp && temaTrend==TrendUp  && volumeTrend!=TrendDown && amaTrend==TrendUp))//
            {
               if(buy_count<bidLimitInTrend && g_bidCountInTrack<nbidLimitInTrack && AtrBuffer[0]>40 && (StdDevBuffer[0]>50||StdDevBuffer[0]>StdDevBuffer[1]) &&
                   AdxPDIBuffer[0]>AdxNDIBuffer[0] && AdxBuffer[0]>AdxBuffer[1] && zigzagTrend!=TrendDown && RsiBuffer[0]<70 && (g_bid<resistance_price2-50))
   //            if(buy_count<bidLimitInTrend && (g_bidCountInTrack<bidLimitInTrack&&StdAtrBidCond) && ((StdDevBuffer[0]>50)||RsiBuffer[0]<70) && zigzagTrend!=TrendDown && candleTrend==TrendUp && ((g_bid<resistance_price2-50&&AtrBuffer[0]<30)||AtrBuffer[0]>30))// && amaTrend!=TrendDown
               {
                  PrintFormat("StdAtrBidCond buy", g_bidCountInTrack);
                  if(amatemaTrading)
                  {
                     g_bidCountInTrend++;
                     g_bidCountInTrack++;
                  }
                  amatemaLastTick = curr_bar_time;
                  amatemaSignal = BuySignalPattern;
                  lastMatchedTrend = TrendUp;
                  return BuySignalPattern;
               }
            }
         }
   }
   return NoSignalPattern;
}

SignalPattern AutoTradedTickets::getAmaTema15SignalPattern(void)
{
   datetime curr_time_origin = TimeCurrent();
   datetime curr_time = RemoveSeconds(curr_time_origin);
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   double sec = double(curr_time_origin-curr_bar_time);

/*   if((AtrBuffer[0]<10 && StdDevBuffer[0]<10))//
      return NoSignalPattern;
   if(StdDevBuffer15[0]<100)
      return NoSignalPattern;
*/   
   bNoBidFor15Min = false;
   int sell_count = countMatchInTrend(SellSignalPattern);
   int buy_count = countMatchInTrend(BuySignalPattern);
   TrendPattern candleTrend = chartAnalyzer.candleTrend;
   double currTema = TEMABuffer5[0];
   double prevTema = TEMABuffer5[1];
   double pprevTema = TEMABuffer5[2];
   long volume2 = iVolume(_symbol, PERIOD_M5, 2);
   double body_len2 = iOpen(_symbol, PERIOD_M5, 2)-iClose(_symbol, PERIOD_M5, 2);
   long volume1 = iVolume(_symbol, PERIOD_M5, 1);
   double body_len1 = iOpen(_symbol, PERIOD_M5, 1)-iClose(_symbol, PERIOD_M5, 1);
   double open0 = iOpen(_symbol, PERIOD_M5, 0);
   double close0 = iClose(_symbol, PERIOD_M5, 0);
   if(((prevTema>AmaBuffer5[1]&&currTema<AmaBuffer5[0])||(prevTema<AmaBuffer5[1]&&currTema>AmaBuffer5[0]))&&sell_count==0&&buy_count==0&&curr_bar_time!=amatemaLastTick)//
   {
      g_bidCountInTrack5 = 0;
   }
   if(resistance_price2-support_price2<250)
   {
      bool amaHorizontalState = true;
      for(int i = 0; i < 6; i++)
      {
         if(AmaBuffer15[i]<resistance_price2&&AmaBuffer15[i]>support_price2&&MathAbs(AmaBuffer15[i]-AmaBuffer15[i+1])<7)
            amaHorizontalState &= true;
         else
            amaHorizontalState &= false;
      }
      if(amaHorizontalState)
         return NoSignalPattern;
   }
   int nbidLimitInTrack = 1;//(MathAbs(TEMABuffer15[0]-TEMABuffer15[1])>200)?bidLimitInTrack+1:bidLimitInTrack;
   if(!(MathAbs(support_price2-resistance_price2)<200 && (AmaBuffer15[0]<resistance_price2&&AmaBuffer15[0]>support_price2)))//
   {
         if(AmaBuffer5[0]>currTema && AmaBuffer5[0]>g_bid && body_len1>0 && ((AdxBuffer[1]>AdxBuffer[2]&&AdxBuffer[0]-AdxBuffer[1]>AdxBuffer[1]-AdxBuffer[2])))// && g_bidCountInTrack>0
         {
            if((temaTrend15==TrendDown && temaTrend==TrendDown && volumeTrend!=TrendUp && amaTrend==TrendDown))//
            {
               if(sell_count<bidLimitInTrend && g_bidCountInTrack5<nbidLimitInTrack && AtrBuffer[0]>40 && (StdDevBuffer[0]>50||StdDevBuffer[0]>StdDevBuffer[1]) && 
                  AdxNDIBuffer[0]>AdxPDIBuffer[0] && AdxBuffer[0]>AdxBuffer[1] && zigzagTrend!=TrendUp && RsiBuffer[0]>30 && (g_bid>support_price2+50))
   //            if(sell_count<bidLimitInTrend && (g_bidCountInTrack<bidLimitInTrack&&StdAtrBidCond) && ((StdDevBuffer[0]>50)||RsiBuffer[0]>30) && zigzagTrend!=TrendUp && candleTrend==TrendDown && ((g_bid>support_price2+50&&AtrBuffer[0]<30)||AtrBuffer[0]>30))// && amaTrend!=TrendUp
               {
                  PrintFormat("StdAtrBidCond5 sell", g_bidCountInTrack);
                  if(amatemaTrading)
                  {
                     g_bidCountInTrend++;
                     g_bidCountInTrack++;
                  }
                  amatemaLastTick = curr_bar_time;
                  amatemaSignal = SellSignalPattern;
                  lastMatchedTrend = TrendDown;
                  return SellSignalPattern;
               }
            }
         }
         else if(AmaBuffer5[0]<currTema && AmaBuffer5[0]<g_bid && body_len1<0 && ((AdxBuffer[1]>AdxBuffer[2]&&AdxBuffer[0]-AdxBuffer[1]>AdxBuffer[1]-AdxBuffer[2])||(TEMABuffer15[0]>TEMABuffer15[1]+200)))// && g_bidCountInTrack>0
         {
            if((temaTrend15==TrendUp && temaTrend==TrendUp  && volumeTrend!=TrendDown && amaTrend==TrendUp))//
            {
               if(buy_count<bidLimitInTrend && g_bidCountInTrack5<nbidLimitInTrack && AtrBuffer[0]>40 && (StdDevBuffer[0]>50||StdDevBuffer[0]>StdDevBuffer[1]) &&
                   AdxPDIBuffer[0]>AdxNDIBuffer[0] && AdxBuffer[0]>AdxBuffer[1] && zigzagTrend!=TrendDown && RsiBuffer[0]<70 && (g_bid<resistance_price2-50))
   //            if(buy_count<bidLimitInTrend && (g_bidCountInTrack<bidLimitInTrack&&StdAtrBidCond) && ((StdDevBuffer[0]>50)||RsiBuffer[0]<70) && zigzagTrend!=TrendDown && candleTrend==TrendUp && ((g_bid<resistance_price2-50&&AtrBuffer[0]<30)||AtrBuffer[0]>30))// && amaTrend!=TrendDown
               {
                  PrintFormat("StdAtrBidCond5 buy", g_bidCountInTrack);
                  if(amatemaTrading)
                  {
                     g_bidCountInTrend++;
                     g_bidCountInTrack++;
                  }
                  amatemaLastTick = curr_bar_time;
                  amatemaSignal = BuySignalPattern;
                  lastMatchedTrend = TrendUp;
                  return BuySignalPattern;
               }
            }
         }
   }
   return NoSignalPattern;
}

SignalPattern AutoTradedTickets::getStableSignalPattern(void)
{
   if(m_LongSpecialCount<longTermSpecialCountLimit)
   {
      if(long_term_trend==1)
      {
         m_LongSpecialCount++;
         return BuySignalPattern;
      }
      else if(long_term_trend==-1)
      {
         m_LongSpecialCount++;
         return SellSignalPattern;
      }
      else if(long_term_trend==0)
      {
         if(g_bid<resistance_price+1000 && g_bid>resistance_price-2000)
         {
            if(longTemaTrend==TrendDown)
            {
         PrintFormat("longtematrend=%d, sell", longTemaTrend);
               return SellSignalPattern;
            }
         }
         else if(g_bid<support_price+2000 && g_bid>support_price-1000)
         {
            if(longTemaTrend==TrendUp)
            {
         PrintFormat("longtematrend=%d, buy", longTemaTrend);
               return BuySignalPattern;
            }
         }      
      }
   }
   return NoSignalPattern;
}

double AutoTradedTickets::getMinMaxTargetPrice(SignalPattern signal)
{
   if(signal==BuySignalPattern && ZIGZAGBuffer[0]!=0 && ZigzagLowBuffer[0]!=0)
   {
      return ZIGZAGBuffer[highIndex[0]]-g_ask;
   }
   else if(signal==SellSignalPattern && ZIGZAGBuffer[0]!=0 && ZigzagHighBuffer[0]!=0)
   {
      return g_bid-ZIGZAGBuffer[lowIndex[0]]-spread;
   }
   return 0;
}

datetime prevMaxMinTime;
SignalPattern AutoTradedTickets::getMaxMinSignalPattern(double &zigzag[], double &high[], double &low[], int &highPoints[], int &lowPoints[])
{
   double max = iHigh(_symbol, PERIOD_CURRENT, 0);
   double min = iLow(_symbol, PERIOD_CURRENT, 0);
   SignalPattern temaSignal = CheckTEMASignal();
   if(max==min)
      return NoSignalPattern;
/*   if(zigzag[0]!=0 && high[0]!=0)
   {
      return BuyCloseSignalPattern;
   }
   if(zigzag[0]!=0 && low[0]!=0)
   {
      return SellCloseSignalPattern;
   }*/
   int buy_count = countMatchInTrend(BuySignalPattern);
   int sell_count = countMatchInTrend(SellSignalPattern);
   if(zigzagTrend!=TrendDown && temaTrend15==TrendUp && temaTrend5==TrendUp && buy_count<bidLimitInTrend && low[0]!=0 && zigzag[0]!=0 && AdxNDIBuffer[0]<AdxNDIBuffer[1])
      return BuySignalPattern;
   else if(zigzagTrend!=TrendUp && temaTrend15==TrendDown && temaTrend5==TrendDown && sell_count<bidLimitInTrend && high[0]!=0 && zigzag[0]!=0 && AdxPDIBuffer[0]<AdxPDIBuffer[1])
      return SellSignalPattern;
/*   if(zigzagTrend==TrendUp&&temaSignal==BuySignalPattern&&buy_count<bidLimitInTrend&&highPoints[0]>lowPoints[0]&&RsiBuffer[0]<70)
      return BuySignalPattern;
   else if(zigzagTrend==TrendDown&&temaSignal==SellSignalPattern&&sell_count<bidLimitInTrend&&lowPoints[0]>highPoints[0]&&RsiBuffer[0]>30)
      return SellSignalPattern;
   if(zigzagTrend==NoTrend&&potentialTrend==PotentialTrendUp&&buy_count<bidLimitInTrend&&temaTrend==TrendUp&&potential_trend_time!=currBarTime&&RsiBuffer[0]<70)
      return BuySignalPattern;
   else if(zigzagTrend==NoTrend&&potentialTrend==PotentialTrendDown&&sell_count<bidLimitInTrend&&temaTrend==TrendDown&&potential_trend_time!=currBarTime&&RsiBuffer[0]>30)
      return SellSignalPattern;*/
   return NoSignalPattern;
}

TrendPattern AutoTradedTickets::DetectLongTermTrend(void)
{
   return NoTrend;
}

void AutoTradedTickets::addPriceToTenPrices(double bid)
{
   for(int i = 1; i < m_ten_prices_count; i++)
   {
      m_ten_prices[i-1] = m_ten_prices[i];
   }
   m_ten_prices[m_ten_prices_count-1] = bid;
}

SignalPattern AutoTradedTickets::getMacdSignalPattern(void)
{
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   double macdCurrent = MACDBuffer[0];
   double macdPrevious = MACDBuffer[1];
   double macdPPrevious = MACDBuffer[2];
   double signalCurrent = MACDSignalBuffer[0];
   double signalPrevious = MACDSignalBuffer[1];
   double signalPPrevious = MACDSignalBuffer[2];
   TrendPattern candleTrend = chartAnalyzer.candleTrend;
   int buy_count = countMatchInTrend(BuySignalPattern);
   int sell_count = countMatchInTrend(SellSignalPattern);
   if(((macdCurrent > signalCurrent && macdPrevious <= signalPrevious)||(macdCurrent < signalCurrent && macdPrevious >= signalPrevious))&&sell_count==0&&buy_count==0&&curr_bar_time!=amatemaLastTick)//
      g_bidCountInTrack = 0;
   if(!m15 && AtrBuffer[0]<50 && StdDevBuffer[0]<50)//
   {
      if(macdCurrent > signalCurrent && macdPrevious <= signalPrevious)
         return SellCloseSignalPattern;
      if(macdCurrent < signalCurrent && macdPrevious >= signalPrevious)
         return BuyCloseSignalPattern;
      return NoSignalPattern;
   }
   if(m15 && AtrBuffer[0]<100 && StdDevBuffer[0]<100)//
      return NoSignalPattern;
   if(bNoBidFor15Min && m15)
   {
      datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
      double sec15 = double(noBidTime-curr_bar_time);
      if(sec15/60 < 15)
      {
         return NoSignalPattern;
      }
      bNoBidFor15Min = false;
   }
   if((!m15&&chartAnalyzer.prev_volatility!=Large&&AdxBuffer[0]>25)||m15)
   {
       if ((MathAbs(signalCurrent) > macdThreshold && macdCurrent > signalCurrent && macdPrevious <= signalPrevious && (macdCurrent-macdPrevious)>0)||
           (MathAbs(signalPrevious) > macdThreshold && macdPrevious > signalPrevious && macdPPrevious <= signalPPrevious && (macdCurrent-macdPrevious)>0))
       {
          if(candleTrend==TrendUp && zigzagTrend!=TrendDown && amaTrend!=TrendDown &&
             ((temaTrend==TrendUp && g_bid<support_price2+AtrBuffer[0]*1.5)||(RsiBuffer[1]<30 && rsiTrend==TrendUp)) && (g_ask<BBUpperBuffer[0]-50))
          {
            if(buy_count<bidLimitInTrend && g_bidCountInTrack<bidLimitInTrack)
            {
               lastMatchedTrend = TrendUp;
               amatemaLastTick = curr_bar_time;
               g_bidCountInTrack++;
/*               if(RsiBuffer[1]<30)
                  PrintFormat("macd buy rsi=%.2f", RsiBuffer[1]);
               else
                  PrintFormat("macd buy tema up", RsiBuffer[1]);*/
               if(m15)
                  bNoBidFor15Min = true;

               return BuySignalPattern;//매수
            }
          }
       }
       else if ((MathAbs(signalCurrent) > macdThreshold && macdCurrent < signalCurrent && macdPrevious >= signalPrevious && (macdPrevious-macdCurrent)>0)||
                (MathAbs(signalPrevious) > macdThreshold && macdPrevious < signalPrevious && macdPPrevious >= signalPPrevious && (macdPrevious-macdCurrent)>0))
       {
           if(candleTrend==TrendDown && zigzagTrend!=TrendUp && amaTrend!=TrendUp &&
             ((temaTrend==TrendDown && g_bid>resistance_price2-AtrBuffer[0]*1.5)||(RsiBuffer[0]>70 && rsiTrend==TrendDown)) && (g_bid>BBLowerBuffer[0]+50))
           {
            if(sell_count<bidLimitInTrend && g_bidCountInTrack<bidLimitInTrack)
            {
               lastMatchedTrend = TrendDown;
               amatemaLastTick = curr_bar_time;
               g_bidCountInTrack++;
/*               if(RsiBuffer[1]>70)
                  PrintFormat("macd sell rsi=%.2f", RsiBuffer[1]);
               else
                  PrintFormat("macd sell tema down", RsiBuffer[1]);*/
               if(m15)
                  bNoBidFor15Min = true;
               return SellSignalPattern;
            }
           }
       }
   }
   if(macdCurrent > signalCurrent && macdPrevious <= signalPrevious)
      return SellCloseSignalPattern;
   if(macdCurrent < signalCurrent && macdPrevious >= signalPrevious)
      return BuyCloseSignalPattern;
    return NoSignalPattern;
}

SignalPattern AutoTradedTickets::getMacd15SignalPattern(void)
{
   double macdCurrent = MACDBuffer5[0];
   double macdPrevious = MACDBuffer5[1];
   double macdPPrevious = MACDBuffer5[2];
   double signalCurrent = MACDSignalBuffer5[0];
   double signalPrevious = MACDSignalBuffer5[1];
   double signalPPrevious = MACDSignalBuffer5[2];
   double macdCurrent15 = MACDBuffer15[0];
   double macdPrevious15 = MACDBuffer15[1];
   double macdPPrevious15 = MACDBuffer15[2];
   double signalCurrent15 = MACDSignalBuffer15[0];
   double signalPrevious15 = MACDSignalBuffer15[1];
   double signalPPrevious15 = MACDSignalBuffer15[2];
   datetime curr_bar_time_m15 = iTime(_symbol, PERIOD_M5, 0);
   datetime curr_bar_time_m1 = iTime(_symbol, PERIOD_M1, 0);
   int buy_count = countMacd15MatchInTrend(BuySignalPattern15);
   int sell_count = countMacd15MatchInTrend(SellSignalPattern15);
   if(AtrBuffer15[0]<100 || StdDevBuffer15[0]<100)//
   {
       if (macdCurrent > signalCurrent && macdPrevious <= signalPrevious)
       {
        if(curr_bar_time_m15!=noBidTime)
             g_bidCountInTrack = 0;
            return SellCloseSignalPattern15;
       }
       else if (macdCurrent < signalCurrent && macdPrevious >= signalPrevious)
       {
        if(curr_bar_time_m15!=noBidTime)
             g_bidCountInTrack = 0;
            return BuyCloseSignalPattern15;
       }
      return NoSignalPattern;
    }
   if(bNoBidForOneMin)
   {
      double sec1 = double(curr_bar_time_m1-noBidTime);
      if((int)(sec1/60) < 1)
      {
         return NoSignalPattern;
      }
      bNoBidForOneMin = false;
   }
   if(bNoBidFor15Min)
   {
      double sec15 = double(curr_bar_time_m15-noBidTime);
      if((int)(sec15/60) <= 5)
      {
         return NoSignalPattern;
      }
      bNoBidFor15Min = false;
   }
    if (MathAbs(signalCurrent) > macdThreshold && macdCurrent > signalCurrent && macdPrevious <= signalPrevious && (macdCurrent-macdPrevious)>3)
    {
       if(AdxBuffer[0]>AdxBuffer[1] && AdxPDIBuffer[0]>AdxPDIBuffer[1] && AdxPDIBuffer5[0]>AdxPDIBuffer5[1] && temaTrend==TrendUp && temaTrend5==TrendUp && temaTrend15!=TrendDown)
       {
          if(buy_count==0 && g_bidCountInTrack==0 && temaTrend==TrendUp && temaTrend5==TrendUp && temaTrend15!=TrendDown)
          {
            if(g_bid>g_prev_bid)
            {
               noBidTime = iTime(_symbol, PERIOD_M5, 0);
               bNoBidForOneMin = true;
               g_bidCountInTrack++;
               return BuySignalPattern15;
            }
          }
          else if(g_bidCountInTrack>0 && buy_count<bidLimitInTrend && g_bidCountInTrack<bidLimitInTrack && temaTrend==TrendUp && temaTrend5==TrendUp && temaTrend15!=TrendDown)
          {
            if((m_nMatchedSignalsCount>0 && g_bid-m_parr_matchedSignals[0].m_ask>0)||m_nMatchedSignalsCount==0)
            {
               if(g_bid<resistance_price2-AtrBuffer[0]*1.5 && g_bid>g_prev_bid)
               {
                    g_bidCountInTrack++;
                  return BuySignalPattern15;
               }
            }
          }
       }
    }
    else if (MathAbs(signalCurrent) > macdThreshold && macdCurrent < signalCurrent && macdPrevious >= signalPrevious && (macdPrevious-macdCurrent)>3)
    {
        if(AdxBuffer[0]>AdxBuffer[1] && AdxNDIBuffer[0]>AdxNDIBuffer[1] && AdxNDIBuffer5[0]>AdxNDIBuffer5[1] && temaTrend==TrendDown && temaTrend5==TrendDown && temaTrend15!=TrendUp)
        {
           if(sell_count==0 && g_bidCountInTrack==0 && temaTrend==TrendDown && temaTrend5==TrendDown && temaTrend15!=TrendUp)
           {
               if(g_bid<g_prev_bid)
               {
                  noBidTime = iTime(_symbol, PERIOD_M5, 0);
                  bNoBidForOneMin = true;
                  g_bidCountInTrack++;
                  return SellSignalPattern15;
               }
           }
           else if(g_bidCountInTrack>0 && sell_count<bidLimitInTrend && g_bidCountInTrack<bidLimitInTrack && temaTrend==TrendDown && temaTrend5==TrendDown && temaTrend15!=TrendUp)
           {
            if((m_nMatchedSignalsCount>0 && m_parr_matchedSignals[0].m_bid-g_ask>0)||m_nMatchedSignalsCount==0)
            {
               if(g_bid>support_price2+AtrBuffer[0]*1.5 && g_bid<g_prev_bid)
               {
                    g_bidCountInTrack++;
                  return SellSignalPattern15;
               }
            }
           }
        }
    }
    if (macdCurrent > signalCurrent && macdPrevious <= signalPrevious)
    {
        if(curr_bar_time_m15!=noBidTime)
         g_bidCountInTrack = 0;
         return SellCloseSignalPattern15;
    }
    else if (macdCurrent < signalCurrent && macdPrevious >= signalPrevious)
    {
        if(curr_bar_time_m15!=noBidTime)
         g_bidCountInTrack = 0;
         return BuyCloseSignalPattern15;
    }
    return NoSignalPattern;
}

SignalPattern AutoTradedTickets::getLeapPattern()
{
   if(prev_average_tick_volume==0||recent_average_tick_volume<5)
      return NoSignalPattern;
   datetime current_time = TimeCurrent();
   int buy_count = countLeapMatchInTrend(BuySignalPattern);
   int sell_count = countLeapMatchInTrend(SellSignalPattern);
   double max = iHigh(_symbol, PERIOD_CURRENT, 0);
   double min = iLow(_symbol, PERIOD_CURRENT, 0);
   bool StdAtrBidCond = checkStdAtrCondition();
   if(StdAtrBidCond && buy_count==0 && temaTrend==TrendUp && double(current_time-average_record_start_time)>60*3 && prev_average_tick_volume*2<recent_average_tick_volume)
      return BuySignalPattern;
   else if(StdAtrBidCond && sell_count==0 && temaTrend==TrendDown && double(current_time-average_record_start_time)>60*3 && prev_average_tick_volume*2<recent_average_tick_volume)
      return SellSignalPattern;
   return NoSignalPattern;
}

void AutoTradedTickets::CloseLongSpecialPosition(void)
{
   int i = 0;
   datetime curr_time = TimeCurrent();
   curr_time = RemoveSeconds(curr_time);
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   SignalPattern prev_signal = NoSignalPattern;
   while(i < m_nMatchedSignalsCount)
   {
      int barCount = (int)(double(curr_bar_time-m_parr_matchedSignals[i].m_time)/(60*1));
      g_position.SelectByTicket(m_parr_matchedSignals[i].m_ticketNumber);
      SignalPattern signal = m_parr_matchedSignals[i].m_kind;
      if(m_parr_matchedSignals[i].m_close_kind!=LongTermSpecialCloseShortKind&&m_parr_matchedSignals[i].m_close_kind!=LongTermSpecialCloseLongKind)// || curr_time==m_parr_matchedSignals[i].m_time
      {
         i++; continue;
      }
      if(barCount>0 && temaClose && g_position.Profit()>0 && ((temaTrend15==TrendDown&&chartAnalyzer.candleTrend==TrendDown&&signal==BuySignalPattern)
         ||(temaTrend15==TrendUp&&chartAnalyzer.candleTrend==TrendUp&&signal==SellSignalPattern)))
      {
         Print("CloseLongSpecialPosition tema close");
         bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
         if(ret)
         {
            m_LongSpecialCount--;
            if(placeOrderWhenClose)
            {
               if(signal==BuySignalPattern)
                  SellOnePosition(LongTermSpecialCloseLongKind);
               else if(signal==SellSignalPattern)
                  BuyOnePosition(LongTermSpecialCloseLongKind);
            }
            continue;         
         }        
      }
/*      else if((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&(g_bid<=BBMiddleBuffer[0])&&temaTrend==TrendDown)
               ||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&(g_ask>=BBMiddleBuffer[0])&&temaTrend==TrendUp))
      {
//            if(g_position.Profit()>0)
         {
            bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               continue;
            }
         }
      }*/
      else
      {
         TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
         if((consistency==TrendDown&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (consistency==TrendUp&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))
         {
            double coeff = ((temaTrend15==TrendUp&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(temaTrend15==TrendDown&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))?
               exp(1+(AdxBuffer[0]-60)/10)*AtrBuffer[0]:exp(1+(AdxBuffer[0]-60)/10)*AtrBuffer[0]*0.5;
            if(m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind)
               coeff = ((temaTrend15==TrendUp&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(temaTrend15==TrendDown&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))?
                  exp(1+(AdxBuffer[0]-60)/10)*AtrBuffer[0]*4:exp(1+(AdxBuffer[0]-60)/10)*AtrBuffer[0]*2;
            double profit = g_position.Profit();
            if(profit>=m_parr_matchedSignals[i].m_lot*coeff)
            {
               if(handBidAutoConsistencyClose && m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind)
               {
                  consistencyCloseLot = m_parr_matchedSignals[i].m_lot;
                  bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  Print("CloseLongSpecialPosition handbidautoconsistencyclose coeff");
                  if(ret)
                  {
                     m_LongSpecialCount--;
                     consistencyClosedTime = curr_bar_time;
                     continue;
                  }
               }
            }
            if(handBidAutoConsistencyClose&&_symbol=="BTCUSD" && (profit<m_parr_matchedSignals[i].m_lot*-50||profit>0)&&((g_bid>resistance_price2-50&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(g_bid<support_price2+50&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
            {
               consistencyCloseLot = m_parr_matchedSignals[i].m_lot;
               bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  Print("CloseLongSpecialPosition handbidautoconsistencyclose resistance support");
                  consistencyClosedTime = curr_bar_time;
                  continue;
               }
            }
         }
/*         if((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&adxTrend==TrendDown)||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&adxTrend==TrendUp))
         {
            bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               m_LongSpecialCount--;
               continue;
            }
         }*/
      }
      if(handBidAutoConstProfitClose)
      {
         if((longTemaTrend!=TrendUp&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (longTemaTrend!=TrendDown&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))
         {
            prev_signal = m_parr_matchedSignals[i].m_kind;
            if((m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind&&g_position.Profit()>=m_parr_matchedSignals[i].m_lot*g_target_profit_s)||
               (m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind&&g_position.Profit()>=m_parr_matchedSignals[i].m_lot*g_target_profit))
            {
               bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  Print("CloseLongSpecialPosition handBidAutoConstProfitClose 1");
                  m_LongSpecialCount--;
                  continue;         
               }        
            }
         }
         else if((m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind&&g_position.Profit()>=m_parr_matchedSignals[i].m_lot*g_target_profit_s*2)||
            (m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind&&g_position.Profit()>=m_parr_matchedSignals[i].m_lot*g_target_profit*2))
         {
            bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
                  Print("CloseLongSpecialPosition handBidAutoConstProfitClose 2");
               m_LongSpecialCount--;
               continue;
            }
         }
      }
/*      if(barCount>1 && g_position.Profit()<m_parr_matchedSignals[i].m_lot*-(stopLossAutoDefault-2))
      {
         if(handBidAutoBid)
         {
            if(m_parr_matchedSignals[i].m_kind==BuySignalPattern){
               SellOneSpecialPosition(true);
            }
            else if(m_parr_matchedSignals[i].m_kind==SellSignalPattern){
               BuyOneSpecialPosition(true);
            }
         }
         if(handBidAutoClose)
         {
            bool ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               m_LongSpecialCount--;
               continue;
            }
         }
      }*/
      i++;
   }
}

void AutoTradedTickets::CloseOrderPositions(SignalPattern kind, bool bProfit=true)
{
   if(bPrintShowz)
      PrintFormat("autoTradedTickets.m_nMatchedSignalsCount=%d", autoTradedTickets.m_nMatchedSignalsCount);
   int i = 0;
   while(i < autoTradedTickets.m_nMatchedSignalsCount)
   {
      if(m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind||m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind||autoTradedTickets.m_parr_matchedSignals[i].m_close_kind==Macd15CloseKind)
      {
         i++;
         continue;
      }
      if(autoTradedTickets.m_parr_matchedSignals[i].m_kind==kind)
      {
         g_position.SelectByTicket(m_parr_matchedSignals[i].m_ticketNumber);
         datetime cur_time = TimeCurrent();
         double diff_time = double(cur_time-m_parr_matchedSignals[i].m_time)/60;
         if(!bProfit||(bProfit&&g_position.Profit()>=0))//300
         {
            if(bPrintShow)
               Print("closeOrder");
            bool ret = trade.PositionClose(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               autoTradedTickets.deleteMatchedSignalByTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
            }
            else
               i++;
         }
         else
            i++;
      }
      else
         i++;
   }
}

void AutoTradedTickets::CloseWrongPositions()
{
   TrendPattern trend = DetermineTrend(ZIGZAGBuffer, ZigzagHighBuffer, ZigzagLowBuffer);
   datetime curr_time_origin = TimeCurrent();
   datetime curr_time = RemoveSeconds(curr_time_origin);
   datetime curr_bar_time = iTime(_symbol, PERIOD_CURRENT, 0);
   double sec = double(curr_time_origin-curr_time);
   int i = 0;
   double max = iHigh(_symbol, PERIOD_CURRENT, 0);
   double min = iLow(_symbol, PERIOD_CURRENT, 0);
   while(i < autoTradedTickets.m_nMatchedSignalsCount)
   {
      int barCount = (m15)?(int)(double(curr_bar_time-m_parr_matchedSignals[i].m_time)/(60*15)):(int)(double(curr_bar_time-m_parr_matchedSignals[i].m_time)/(60));//(int)(double(curr_time-autoTradedTickets.m_parr_matchedSignals[i].m_time))/60*15;
      int secondInNewBar = (int)(double)(curr_time_origin-curr_time);
      SignalPattern temaSignal = CheckTEMASignal();
      g_position.SelectByTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
      bool ret = false;
/*         if(m_parr_matchedSignals[i].m_kind==BuySignalPattern && temaSignal==SellSignalPattern)
         {
            CloseBuyPositions();
            return;
         }
         else if(m_parr_matchedSignals[i].m_kind==SellSignalPattern && temaSignal==BuySignalPattern)
         {
            CloseSellPositions();
            return;
         }*/
      if(autoTradedTickets.m_parr_matchedSignals[i].m_close_kind==ZigzagCloseKind)
      {
/*         if((autoTradedTickets.m_parr_matchedSignals[i].m_macd < 0 && autoTradedTickets.m_parr_matchedSignals[i].m_kind==SellSignalPattern)||
            (autoTradedTickets.m_parr_matchedSignals[i].m_macd > 0 && autoTradedTickets.m_parr_matchedSignals[i].m_kind==BuySignalPattern))
         {
            if(g_position.Profit()>0.1)
            {
               if(bPrintShow)
                  Print("Called if((autoTradedTickets.m_parr_matchedSignals[i].m_macd < 0"); 
               ret = CloseTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
                  continue;
            }
         }
         else if((autoTradedTickets.m_parr_matchedSignals[i].m_bid<autoTradedTickets.m_high && autoTradedTickets.m_parr_matchedSignals[i].m_kind==SellSignalPattern)
          || (autoTradedTickets.m_parr_matchedSignals[i].m_bid>autoTradedTickets.m_low && autoTradedTickets.m_parr_matchedSignals[i].m_kind==BuySignalPattern))
         {
            if(g_position.Profit()>0.1)
            {
               if(bPrintShow)
                  Print("Called else if((autoTradedTickets.m_parr_matchedSignals[i].m_bid<autoTradedTickets.m_high"); 
               ret = CloseTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
                  continue;
            }
         }
         else if(barCount>1 && barCount<=autoTradedTickets.m_parr_matchedSignals[i].m_afterBarCountToCheck)
         {
            if(autoTradedTickets.m_parr_matchedSignals[i].m_kind==BuySignalPattern)
            {
               if(temaSignal==SellSignalPattern && g_position.Profit()>0)
               {
                  if(bPrintShow)
                     Print("Called if(temaSignal==SellSignalPattern)");
                  ret = CloseTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }
            }
            else if(autoTradedTickets.m_parr_matchedSignals[i].m_kind==SellSignalPattern)
            {
               if(temaSignal==BuySignalPattern && g_position.Profit()>0)
               {
                  if(bPrintShow)
                     Print("Called if(temaSignal==BuySignalPattern)");
                  ret = CloseTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                     continue;
               }
            }
         }
         else if(barCount==0)
         {
            if(g_position.Profit()>=0.1)
            {
               ret = CloseTicket(autoTradedTickets.m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
                  continue;
            }
         }*/
      }
      else if(m_parr_matchedSignals[i].m_close_kind==LeapCloseKind)
      {
         TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
/*         if(barCount==0 && (g_position.Profit()>=m_parr_matchedSignals[i].m_lot*50))
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
               continue;
         }*/
/*         if(g_position.Profit()>m_parr_matchedSignals[i].m_lot*500||(temaClose&&m_parr_matchedSignals[i].m_kind==BuySignalPattern&&temaTrend==TrendDown)||
            (temaClose&&m_parr_matchedSignals[i].m_kind==SellSignalPattern&&temaTrend==TrendUp))
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
               continue;
         }*/
         if(autoBidAutoConsistencyClose && ((consistency==TrendDown&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (consistency==TrendUp&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
         {
            if(g_position.Profit()>m_parr_matchedSignals[i].m_lot*500)
            {
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  g_bidCountInTrend--;
                  continue;
               }
            }
         }
/*         else if(g_position.Profit()<-m_parr_matchedSignals[i].m_lot*(spread*2))
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               g_bidCountInTrend--;
               continue;
            }
         }*/
      }
/*      else if(m_parr_matchedSignals[i].m_close_kind==MacdCloseKind)
      {
         TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
         if(barCount==0 && ((consistency==TrendDown&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (consistency==TrendUp&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
         {
            if(g_position.Profit()>m_parr_matchedSignals[i].m_lot*10)
            {
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  g_bidCountInTrend--;
                  continue;
               }
            }
         }
         if(barCount>0 && ((m_parr_matchedSignals[i].m_kind==BuySignalPattern && temaTrend15==TrendDown)||
            (m_parr_matchedSignals[i].m_kind==SellSignalPattern && temaTrend==TrendUp)))
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
               continue;
         }
      }*/
      else if(m_parr_matchedSignals[i].m_close_kind==MaxMinCloseKind)
      {
         double profit = g_position.Profit();
         if(barCount<m_parr_matchedSignals[i].m_afterBarCountToCheck && max!=min)
         {
            double coeff = exp(1+(AdxBuffer[0]-60)/10)*100;
            TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
            if(((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&consistency==TrendDown)||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&consistency==TrendUp)))
            {
               if(profit>=m_parr_matchedSignals[i].m_lot*m_parr_matchedSignals[i].m_target_profit)
               {
                  ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                  {
                     continue;
                  }
               }
            }
            else
            {
               if((ZIGZAGBuffer[0]!=0 && ZigzagHighBuffer[0]!=0 && m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
                  (ZIGZAGBuffer[0]!=0 && ZigzagLowBuffer[0]!=0 && m_parr_matchedSignals[i].m_kind==SellSignalPattern))
               {
                  if(profit > 0)
                  {
                     ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                     if(ret)
                     {
                        continue;
                     }
                  }
               }
               if((((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&temaTrend==TrendDown)||
                  (m_parr_matchedSignals[i].m_kind==SellSignalPattern&&temaTrend==TrendUp))))
               {
                  if(profit>=0)
                  {
                     ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                     if(ret)
                     {
                        continue;
                     }
                  }
               }
            }
            if(g_position.Profit()>=m_parr_matchedSignals[i].m_lot*100)
            {
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
                  continue;
            }
         }
         else if(barCount>=m_parr_matchedSignals[i].m_afterBarCountToCheck)
         {
            if((m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(m_parr_matchedSignals[i].m_kind==SellSignalPattern))
            {
               if(g_position.Profit()>0)
               {
                  ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
                  if(ret)
                  {
                     g_bidCountInTrend--;
                     continue;
                  }
               }
            }
         }
      }
      else if(m_parr_matchedSignals[i].m_close_kind==MacdCloseKind||m_parr_matchedSignals[i].m_close_kind==AmaTemaCloseKind||m_parr_matchedSignals[i].m_close_kind==AmaTemaNextCloseKind)
      {
           TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
//         double ama_diff = MathAbs(AmaBuffer[barCount]-AmaBuffer[barCount+1]);
//         double amatema_diff = MathAbs(AmaBuffer[barCount]-TEMABuffer15[barCount]);
         double tema_diff = (m15)?MathAbs(TEMABuffer15[0]-TEMABuffer15[1]):MathAbs(TEMABuffer[0]-TEMABuffer[1]);
         double limitProfit = tema_diff*0.5;
//         double limitProfit = (ama_diff<amaStepThreshold||amatema_diff<amatemaDiffThreshold/2)?10:(ama_diff<=amaStepThreshold*5||amatema_diff<=amatemaDiffThreshold)?
//                     50:(ama_diff<=amaStepThreshold*10||amatema_diff<60)?100:500;
         double tema0 = (m15)?TEMABuffer15[0]:TEMABuffer[0];
         double tema1 = (m15)?TEMABuffer15[1]:TEMABuffer[1];
/*         if(g_position.Profit()>=m_parr_matchedSignals[i].m_lot*limitProfit*2)
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
               continue;
         }*/
         double profit = g_position.Profit();
         double stop = 0;
         g_position.InfoDouble(POSITION_SL, stop);
         stop = (m_parr_matchedSignals[i].m_kind==BuySignalPattern)?(m_parr_matchedSignals[i].m_ask-stop):(stop-m_parr_matchedSignals[i].m_bid);
         if(autoBidAutoClose && stop!=0 && profit<m_parr_matchedSignals[i].m_lot*-stop)
         {
            Print("-50 closed");
            SignalPattern signal = (m_parr_matchedSignals[i].m_kind==BuySignalPattern)?SellSignalPattern:BuySignalPattern;
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               g_bidCountInTrend--;
               if(placeOrderWhenClose)
               {
                  if(signal==SellSignalPattern)
                     SellOnePosition(AmaTemaCloseKind);
                  else if(signal==BuySignalPattern)
                     BuyOnePosition(AmaTemaCloseKind);
               }
               continue;
            }
         }
         /*if(profit>=m_parr_matchedSignals[i].m_lot*200)//StdDevBuffer15[0],AtrBuffer[0]*AtrBuffer[0]*0.05
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               continue;
            }
         }*/
         if(profit>=m_parr_matchedSignals[i].m_lot*m_parr_matchedSignals[i].m_target_profit)
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               continue;
            }
         }
/*         if((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&adxTrend==TrendDown)||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&adxTrend==TrendUp))
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               continue;
            }
         }*/
         if((autoBidAutoConsistencyClose) && ((consistency==TrendDown&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (consistency==TrendUp&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
         {
            double coeff = ((temaTrend15==TrendUp&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(temaTrend15==TrendDown&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))?
               exp(1+(AdxBuffer[0]-60)/10)*100:exp(1+(AdxBuffer[0]-60)/10)*60;
            if((g_bidCountInTrack==1 && profit>m_parr_matchedSignals[i].m_lot*coeff)||(g_bidCountInTrack>1 && profit>m_parr_matchedSignals[i].m_lot*(coeff*0.5)))
            {
               consistencyCloseLot = m_parr_matchedSignals[i].m_lot;
               Print("closeWrongPositions autoBidAutoConsistencyClose coeff");
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  g_bidCountInTrend--;
                  consistencyClosedTime = curr_bar_time;
                  continue;
               }
            }
            if((profit<m_parr_matchedSignals[i].m_lot*-50||profit>0)&&((g_bid>resistance_price2-50&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(g_bid<support_price2+50&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
            {
               consistencyCloseLot = m_parr_matchedSignals[i].m_lot;
               Print("closeWrongPositions resistance support close");
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  g_bidCountInTrend--;
                  consistencyClosedTime = curr_bar_time;
                  continue;
               }
            }
         }
/*         else if((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&(g_bid<=BBMiddleBuffer[0])&&temaTrend==TrendDown)
                  ||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&(g_ask>=BBMiddleBuffer[0])&&temaTrend==TrendUp))
         {
//            if(g_position.Profit()>0)
            {
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  g_bidCountInTrend--;
                  continue;
               }
            }
         }*/
         if(barCount>0 && stdClose && StdDevBuffer[0]<StdDevBuffer[1])
         {
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               g_bidCountInTrend--;
               continue;
            }
         }
         if(barCount>0 && temaClose && m_parr_matchedSignals[i].m_close_kind==AmaTemaCloseKind && ((m_parr_matchedSignals[i].m_kind==BuySignalPattern&&((!m15&&temaTrend15==TrendDown)||(m15&&temaTrend15==TrendDown)))||(m_parr_matchedSignals[i].m_kind==SellSignalPattern&&((!m15&&temaTrend15==TrendUp)||(m15&&temaTrend15==TrendUp)))))
         {
            bool wrong = false;
            SignalPattern signal = (m_parr_matchedSignals[i].m_kind==BuySignalPattern)?SellSignalPattern:BuySignalPattern;
            if(g_position.Profit()<0)
               wrong = true;
            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               g_bidCountInTrend--;
               if(placeOrderWhenClose)
               {
                  if(signal==SellSignalPattern)
                     BuyOnePosition(AmaTemaCloseKind);
                  else if(signal==BuySignalPattern)
                     SellOnePosition(AmaTemaCloseKind);
               }
               continue;
            }
         }
      }
      else if(m_parr_matchedSignals[i].m_close_kind==Macd15CloseKind)
      {
         TrendPattern consistency = chartAnalyzer.checkPriceDiffTrend();
         double profit = g_position.Profit();
         if(profit>=m_parr_matchedSignals[i].m_lot*500)//StdDevBuffer15[0],AtrBuffer[0]*AtrBuffer[0]*0.05
         {
            Print("closeWrongPositions Macd15CloseKind 500");

            ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
            if(ret)
            {
               continue;
            }
         }
         if((autoBidAutoConsistencyClose) && ((consistency==TrendDown&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||
            (consistency==TrendUp&&m_parr_matchedSignals[i].m_kind==SellSignalPattern)))
         {
            double coeff = ((temaTrend15==TrendUp&&m_parr_matchedSignals[i].m_kind==BuySignalPattern)||(temaTrend15==TrendDown&&m_parr_matchedSignals[i].m_kind==SellSignalPattern))?
               exp(1+(AdxBuffer[0]-60)/10)*500:exp(1+(AdxBuffer[0]-60)/10)*300;
            if(profit>m_parr_matchedSignals[i].m_lot*coeff)
            {
               Print("closeWrongPositions Macd15CloseKind autoBidAutoConsistencyClose coeff");
               ret = CloseTicket(m_parr_matchedSignals[i].m_ticketNumber);
               if(ret)
               {
                  continue;
               }
            }
         }
      }
      i++;
   }
   if(g_bidCountInTrend<0)
      g_bidCountInTrend = 0;
}

TrendPattern AutoTradedTickets::DetermineTrend(double &zigzag[], double &high[], double &low[])
{
   TrendPattern trend = NoTrend;
   if(zigzag[0]!=0 && zigzag[0]==high[0]) // && tema5Trend==TrendUp
      trend = TrendUp;
   else if(zigzag[0]!=0 && zigzag[0]==low[0]) // && tema5Trend==TrendDown
      trend = TrendDown;
   return trend;
}

TrendPattern AutoTradedTickets::DetermineTrendByAMA(double &ama[])
{
   double diff = ama[0]-ama[1];
   if(diff>10)
      return TrendUp;
   else if(diff<-10)
      return TrendDown;
   return NoTrend;
}

TrendPattern AutoTradedTickets::DetermineTrendByZigzag(double &zigzag[], double &high[], double &low[], int &highPoints[], int &lowPoints[]){
    int i = 0;
    int curr_direction = 0;
    int high_point_count = 0, low_point_count = 0;
    double currValue = g_bid;
    for(i = 0; i < 100; i++){
        if(zigzag[i]!=0 && curr_direction==0){
            if(high[i]!=0)
            {
                highPoints[high_point_count++] = i;
            }
            else if(low[i]!=0)
            {
                lowPoints[low_point_count++] = i;
            }
        } else if(zigzag[i]!=0){
            if(high[i]!=0)
            {
                highPoints[high_point_count++] = i;
            }
            else if(low[i]!=0)
            {
                lowPoints[low_point_count++] = i;
            }
        } else if(i==99){
            if(high[0]!=0||high[1]!=0 || low[0]!=0||low[1]!=0)
            {
               if(high_point_count==0 && low_point_count==0){
                  return NoTrend;
               } else if(high_point_count==1 && low_point_count==0){
                   if(bPrintShowz)
                       Print("zigzag=1, else if(high_point_count==1 && low_point_count==0)");
                   return TrendUp;
               } else if(low_point_count==1 && high_point_count==0){
                   if(bPrintShowz)
                       Print("zigzag=-1, else if(low_point_count==1 && high_point_count==0)");
                   return TrendDown;
               } else if(high_point_count==1 && low_point_count==1){
                   
                   if(highPoints[0] < lowPoints[0])
                   {
                       if(bPrintShowz)
                           Print("zigzag=-1, else if(high_point_count==1 && low_point_count==1)");
                       if(zigzag[highPoints[0]]>zigzag[99])
                          return TrendUp;
                       else
                          return TrendDown;
                   }
                   else
                   {
                       if(bPrintShowz)
                           Print("zigzag=1, else if(high_point_count==1 && low_point_count==1)");
                       if(zigzag[lowPoints[0]]<zigzag[99])
                          return TrendDown;
                       else
                          return TrendUp;
                   }
               } else if(high_point_count==2 && low_point_count==1){
                   
                   if(zigzag[highPoints[0]]>zigzag[highPoints[1]])
                   {
                       if(bPrintShowz)
                           Print("zigzag=1, else if(high_point_count==2 && low_point_count==1)");
                       return TrendUp;
                   }
                   else if(zigzag[highPoints[0]]<zigzag[highPoints[1]])
                   {
                       if(bPrintShowz)
                           Print("zigzag=-1, else if(high_point_count==2 && low_point_count==1)");
                       return TrendDown;
                   }
               } else if(low_point_count==2 && high_point_count==1){
                   
                   if(zigzag[lowPoints[0]]>zigzag[lowPoints[1]])
                   {
                       if(bPrintShowz)
                           Print("zigzag=1, else if(low_point_count==2 && high_point_count==1)");
                       return TrendUp;
                   }
                   else if(zigzag[lowPoints[0]]<zigzag[lowPoints[1]])
                   {
                       if(bPrintShowz)
                           Print("zigzag=-1, else if(low_point_count==2 && high_point_count==1)");
                       return TrendDown;
                   }
               }
            }
        }
        if(low_point_count>=2 && high_point_count>=2)
            break;
    }
    if(low_point_count>=2 && high_point_count>=2 && (zigzag[0]!=0||zigzag[1]!=0) && (low[0]!=0||low[1]!=0||high[0]!=0||high[1]!=0)){ //장기트렌드를 고려하여 다시 수정해야
        if(highPoints[0]<lowPoints[0] && zigzag[highPoints[0]]>zigzag[highPoints[1]] && zigzag[lowPoints[0]]>zigzag[lowPoints[1]])
        {
            if(bPrintShow)
                Print("zigzag=1, if((high[0]!=0||high[1]!=0) && zigzag[highPoints[0]]>zigzag[highPoints[1]] && zigzag[lowPoints[0]]>zigzag[lowPoints[1]])");
            return TrendUp;
        }
        else if(lowPoints[0]<highPoints[0] && zigzag[highPoints[0]]<zigzag[highPoints[1]] && zigzag[lowPoints[0]]<zigzag[lowPoints[1]])
        {
            if(bPrintShow)
                Print("zigzag=-1, else if((low[0]!=0||low[1]!=0) && zigzag[highPoints[0]]<zigzag[highPoints[1]] && zigzag[lowPoints[0]]<zigzag[lowPoints[1]])");
            return TrendDown;
        }
    }
    if(low_point_count>=2 && high_point_count>=2)
    {
        if(low_point_count>=5 && high_point_count>=5 && MathAbs(zigzag[highPoints[1]]-zigzag[lowPoints[1]])<70)
         return NoTrend;
        if(lowPoints[0]<highPoints[0] && zigzag[lowPoints[0]]<zigzag[lowPoints[1]])
         return TrendDown;
        else if(highPoints[0]<lowPoints[0] && zigzag[highPoints[0]]>zigzag[highPoints[1]])
         return TrendUp;
        if(zigzag[highPoints[0]]>zigzag[lowPoints[0]] && lowPoints[0]>highPoints[0] && lowPoints[1]<highPoints[1])
         return TrendUp;
        else if(zigzag[lowPoints[0]]<zigzag[highPoints[0]] && lowPoints[0]<highPoints[0] && lowPoints[1]>highPoints[1])
         return TrendDown;
        
        if(zigzag[highPoints[0]]>zigzag[highPoints[1]] && zigzag[lowPoints[0]]>zigzag[lowPoints[1]] && lowPoints[0]<highPoints[0] &&
          temaTrend15==TrendUp&&temaTrend==TrendUp&&TEMABuffer[1]>TEMABuffer[2])
         return TrendUp;
/*        if(zigzag[highPoints[0]]>zigzag[highPoints[1]] && zigzag[lowPoints[0]]>zigzag[lowPoints[1]] && lowPoints[0]<highPoints[0] &&
          zigzag[lowPoints[0]]<(zigzag[lowPoints[1]]+(zigzag[highPoints[0]]-zigzag[lowPoints[1]])/3))
         return TrendDown;*/
        else if(zigzag[highPoints[0]]<zigzag[highPoints[1]] && zigzag[lowPoints[0]]<zigzag[lowPoints[1]] && highPoints[0]<lowPoints[0] &&
          temaTrend15==TrendDown&&temaTrend==TrendDown&&TEMABuffer[1]<TEMABuffer[2])
         return TrendDown;
/*        else if(zigzag[highPoints[0]]<zigzag[highPoints[1]] && zigzag[lowPoints[0]]<zigzag[lowPoints[1]] && highPoints[0]<lowPoints[0] &&
          zigzag[highPoints[0]]>(zigzag[highPoints[1]]+(zigzag[lowPoints[0]]-zigzag[highPoints[1]])/3))
         return TrendUp;*/
        if(high_point_count>=3&&low_point_count>=2)
        {
         if(zigzag[highPoints[2]]>(zigzag[highPoints[1]]+50)&&zigzag[highPoints[0]]>zigzag[highPoints[1]]&&MathAbs(zigzag[lowPoints[0]]-zigzag[lowPoints[1]])<50)
         {
            potentialTrend = PotentialTrendUp;//w pattern
            potential_trend_time = currBarTime;
         }
        }
        else if(high_point_count==2&&low_point_count==2)
        {
         if(zigzag[highPoints[0]]>zigzag[highPoints[1]]&&MathAbs(zigzag[lowPoints[0]]-zigzag[lowPoints[1]])<50)
         {
            potentialTrend = PotentialTrendUp;
            potential_trend_time = currBarTime;
         }
        }
        if(low_point_count>=3&&high_point_count>=2)
        {
         if(zigzag[lowPoints[2]]<(zigzag[lowPoints[1]]-50)&&zigzag[lowPoints[0]]<zigzag[lowPoints[1]]&&MathAbs(zigzag[highPoints[0]]-zigzag[highPoints[1]])<50)
         {
            potentialTrend = PotentialTrendDown;//m pattern
            potential_trend_time = currBarTime;
         }
        }
        else if(low_point_count==2&&high_point_count==2)
        {
         if(zigzag[lowPoints[0]]<zigzag[lowPoints[1]]&&MathAbs(zigzag[highPoints[0]]-zigzag[highPoints[1]])<50)
         {
            potentialTrend = PotentialTrendDown;
            potential_trend_time = currBarTime;
         }
        }
    }
    return NoTrend;
}

void AutoTradedTickets::recalcMatchedSignals(void)
{
   int i = 0;
   m_LongSpecialCount = 0;
   g_bidCountInTrend = 0;
   while(i < m_nMatchedSignalsCount)
   {
      ulong ticketNumber = m_parr_matchedSignals[i].m_ticketNumber;
      bool b = g_position.SelectByTicket(ticketNumber);
      if(!b)
      {
         deleteMatchedSignalById(i);
      }
      else 
      {
         if(m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseShortKind||m_parr_matchedSignals[i].m_close_kind==LongTermSpecialCloseLongKind)
            m_LongSpecialCount++;
         if(m_parr_matchedSignals[i].m_close_kind==MaxMinCloseKind||m_parr_matchedSignals[i].m_close_kind==AmaTemaCloseKind)
            g_bidCountInTrend++;
         i++;
      }
   }
}

MatchedSignal* AutoTradedTickets::getLastMatchedSignal(CloseKind kind, SignalPattern signalPattern)
{
   if(m_nMatchedSignalsCount > 0)
   {
      for(int i=m_nMatchedSignalsCount-1; i>=0; i--)
      {
         if(m_parr_matchedSignals[i].m_close_kind==kind && m_parr_matchedSignals[i].m_kind==signalPattern)
            return m_parr_matchedSignals[i];
      }
   }
   return NULL;
}

MatchedSignal* AutoTradedTickets::getLastSignal(void)
{
   return m_parr_matchedSignals[m_nMatchedSignalsCount-1];
}

int AutoTradedTickets::deleteMatchedSignalById(int id)
{
   if(id < 0 || id >= m_nMatchedSignalsCount)
      return -1;
   if(m_parr_matchedSignals[id].m_close_kind==MacdCloseKind)
      m_macdMatchedSignal = NULL;
   else if(m_parr_matchedSignals[id].m_close_kind==LeapCloseKind)
      m_leapMatchedSignal = NULL;
   delete m_parr_matchedSignals[id];
   for(int i = id+1; i < m_nMatchedSignalsCount; i++)
   {
      m_parr_matchedSignals[i-1] = m_parr_matchedSignals[i];
   }
   m_nMatchedSignalsCount--;
   return m_nMatchedSignalsCount;
}

int AutoTradedTickets::deleteMatchedSignalByTicket(ulong ticketNumber)
{
   int id = findMatchedSignal(ticketNumber);
   if(id==-1)
   {
      return -1;
   }
   return deleteMatchedSignalById(id);
}

int AutoTradedTickets::findMatchedSignal(ulong ticketNumber)
{
   for(int i = 0; i < m_nMatchedSignalsCount; i++)
   {
      if(m_parr_matchedSignals[i].m_ticketNumber==ticketNumber)
         return i;
   }
   return -1;
}

int AutoTradedTickets::addMatchedSignal(SignalPattern kind,double ask,double bid,int afterBarCountToCheck, CloseKind closeKind, ulong ticketNumber, double lot, double target_profit)
{
   if(m_nMatchedSignalsCount>=100){
      ArrayResize(m_parr_matchedSignals, m_nMatchedSignalsCount+1);
   }
   if(kind==BuySignalPattern && bid<m_low)
      m_low = bid;
   else if(kind==SellSignalPattern && bid>m_high)
      m_high = bid;
   MatchedSignal* newMatchedSignal = new MatchedSignal(kind, ask, bid, afterBarCountToCheck, closeKind, ticketNumber, lot, target_profit);
   m_parr_matchedSignals[m_nMatchedSignalsCount++] = newMatchedSignal;
   if(closeKind==MacdCloseKind)
      m_macdMatchedSignal = newMatchedSignal;
   else if(closeKind==LeapCloseKind)
      m_leapMatchedSignal = newMatchedSignal;
   return m_nMatchedSignalsCount;
}

double AutoTradedTickets::getCurrBalance(void)
{
   double balance = AccountInfoDouble(ACCOUNT_BALANCE);
   return balance;
}

double AutoTradedTickets::getCurrEquity(void)
{
   double equity = AccountInfoDouble(ACCOUNT_EQUITY);
   return equity;
}

double AutoTradedTickets::getMargin(void)
{
   double margin = AccountInfoDouble(ACCOUNT_MARGIN);
   return margin;
}

double AutoTradedTickets::getFreeMargin(void)
{
   double free_margin = AccountInfoDouble(ACCOUNT_MARGIN_FREE);
   return free_margin;
}

string ArrayToString(const double &array[])
{
   string result = "[";

   for(int i = 0; i < ArraySize(array); i++)
     {
      if(i != 0)
        result += ", ";
      if(array[i]!=EMPTY_VALUE)
         result += DoubleToString(array[i], 2); // 소수점 이하 2자리로 변환
      else
         result += "EMPTY";
     }

   result += "]";
   return(result);
}
  
string ArrayIntegerToString(const int &array[])
{
   string result = "[";

   for(int i = 0; i < ArraySize(array); i++)
     {
      if(i != 0)
        result += ", ";
      result += IntegerToString(array[i]); // 소수점 이하 2자리로 변환
     }

   result += "]";
   return(result);
}
  
datetime RemoveSeconds(datetime time)
{
    MqlDateTime tm;
    TimeToStruct(time, tm);
   // 시간 요소들을 추출
   int year = tm.year;
   int month = tm.mon;
   int day = tm.day;
   int hour = tm.hour;
   int minute = tm.min;

   // 초를 제거하고 새로운 datetime 값 생성
   datetime timeWithoutSeconds;
   timeWithoutSeconds = StringToTime(StringFormat("%04d.%02d.%02d %02d:%02d:00", year, month, day, hour, minute));
   
   return timeWithoutSeconds;
}

int TimeDay(datetime time)
{
   MqlDateTime structTime;
   TimeToStruct(time, structTime);
   return structTime.day;
}

int TimeHour(datetime time)
{
   MqlDateTime structTime;
   TimeToStruct(time, structTime);
   return structTime.hour;
}