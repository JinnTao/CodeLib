#include <cMarketDataCollection.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <cDateTime.h>


cMarketDataCollection::cMarketDataCollection()
{

}

cMarketDataCollection::~cMarketDataCollection()
{

}


cMarketData* cMarketDataCollection::GetMarketDataHandle( const string& name )
{
	cMarketDataPtr p = GetMarketDataHandleSharedPtr( name );
	if( p )
		return p.get();
	else
		return NULL;
}

cMarketDataPtr cMarketDataCollection::GetMarketDataHandleSharedPtr( const string& name )
{
	marketdataHandle::iterator it = _m_mkt_handle.find( name );
	if( it != _m_mkt_handle.end() )
		return (*it).second;
	else
		return cMarketDataPtr();
}




void cMarketDataCollection::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	string id( pDepthMarketData->InstrumentID );
	cMarketData* md = GetMarketDataHandle( id );

	if( !md ){
		// create new CmarketData
		shared_ptr< cMarketData > ptr = make_shared< cMarketData >( id);
		ptr->OnRtnDepthMarketData(pDepthMarketData);
		_m_mkt_handle.insert(map< string, cMarketDataPtr >::value_type(id,ptr));
	}else{
		md->OnRtnDepthMarketData( pDepthMarketData );
	}
}
// ��ȡ��ʷ����
void cMarketDataCollection::loadSeriesHistory(string inst,string startDate,string endDate,vector<double>& open,vector<double> &high,vector<double> &low,vector<double> &close,vector<double> &volume){
	try{
		fstream dataFile;
		string fileName = startDate;
		string date,time;
		double dOpen,dHigh,dLow,dClose,dVolume;

		dataFile.open("dataBase/"+ inst +"/" + fileName + "_1m.txt", ios::_Nocreate | ios::in) ;
		if(dataFile)
		{
			while(!dataFile.eof()){
				dataFile >> date >> time>> dOpen >> dHigh>> dLow>>dClose>>dVolume;
				cerr << date<< " " << time<< " " << dOpen<< " "  << dHigh << " "<< dLow << " "<< dClose<< " " << dVolume <<endl;
				open.push_back(dOpen);
				high.push_back(dHigh);
				low.push_back(dLow);
				close.push_back(dClose);
				volume.push_back(dVolume);
			}
			dataFile.close();	
		}

	}
	catch(...){
	
	}

}
vector<cCandle> cMarketDataCollection::loadCandleHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){
	vector<cCandle> t;
	return t;
}