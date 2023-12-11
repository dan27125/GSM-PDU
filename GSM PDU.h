//-----------------------------------------------------------------------
// filename:	GSM PDU.H
//
// Purpose:     GSM PDU User data decoder for Multitech GSM Modems
//
// Author       Daniel Ambrose, St. Louis, MO, USA
//              email: dan27125@gmail.com
//
//-----------------------------------------------------------------------


#if !defined(_GSM_PDU_H)
#define _GSM_PDU_H


#include <iostream>
#include <fstream>
using namespace std;

    class GsmPDU
    {
    public:
      GsmPDU();
      ~GsmPDU();
    	string	 		pdu;
        unsigned char   ud8[1024];  // type-1, 8 bit classic ascii only
        wchar_t		   	ud16[4096]; // type-2 wchar_t windows format
		UnicodeString 	us;         // type-3 UnicodeString format
    	AnsiString 		as;         // type-4 AnsiString format

    	unsigned int    size;
        unsigned int    size16;
    public:
        void reset();
      	int decode_UD(char *pdu);
        void putc(unsigned short int uc16);
    };

    boolean is_Ascii_Hex_String(char *text);

	std::string to_utf8(const wchar_t* buffer, int len);
	std::string to_utf8(const std::wstring& str);


#endif
