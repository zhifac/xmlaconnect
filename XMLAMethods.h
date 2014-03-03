#import "gSoapTypes.gsoap"
#import "Properties.gsoap"
#import "Restrictions.gsoap"

//gsoap cxmla service namespace: urn:schemas-microsoft-com:xml-analysis
//gsoap sql service namespace: urn:schemas-microsoft-com:xml-sql

#import "DiscoverResponse.gsoap"
#import "ExecuteResponse.gsoap"


struct SOAP_ENV__Header { 
	struct BSessionType {
		xsd__string		@mustUnderstand;
		xsd__string		@xmlns;
		xsd__string		element; 
	} *BeginSession;
	
	struct SessionType {
		xsd__string		@SessionId;
		xsd__string		@mustUnderstand;
		xsd__string		@xmlns;
		xsd__string		element; 
	} *Session;

	struct ESessionType {
		xsd__string		@SessionId;
		xsd__string		@mustUnderstand;
		xsd__string		@xmlns;
		xsd__string		element; 
	} *EndSession;
};



//gsoap xmlns service name: XMLAConnection //gsoap xmlns service type: XMLAConnectionPortType 
//gsoap xmlns service namespace: urn:schemas-microsoft-com:xml-analysis

//gsoap xmlns service method-style: Discover rpc 
//gsoap xmlns service method-encoding: Discover http://schemas.xmlsoap.org/soap/encoding/ 
//gsoap xmlns service method-action: Discover "" 
int xmlns__Discover( char* RequestType, xmlns__Restrictions Restrictions, xmlns__Properties Properties, cxmla__DiscoverResponse &result);

//gsoap xmlns service method-style: Execute rpc 
//gsoap xmlns service method-encoding: Execute http://schemas.xmlsoap.org/soap/encoding/ 
//gsoap xmlns service method-action: Execute "" 
int xmlns__Execute( xsd__string RequestType, xmlns__Command Command, xmlns__Properties Properties, cxmla__ExecuteResponse &result );

