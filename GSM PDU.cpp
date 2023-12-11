//-----------------------------------------------------------------------
// filename:	GSM PDU.CPP
//
// Purpose:     GSM PDU User data decoder for Multitech GSM Modems
//
// Author       Daniel Ambrose, St. Louis, MO, USA
//              email: dan27125@gmail.com
//
//-----------------------------------------------------------------------


#if defined(__BORLANDC__)
#include <vcl.h>        // Header file for Rad Studio C++ Builder
#pragma hdrstop
#endif

#include <strstream.h>
#include <iomanip.h>
#include <algorithm>    // std::transform

#include "GSM PDU.h"


#if defined(DOCUMENTATION)

    When using a Multitec GSM modem in text mode i.e. AT+CMGF=1  the user
    data can appear in two different formats. Unicode vs classic 8 bit ascii.

    if the SMS message includes any unicode 16 bit chracters such as emoticons
    or any non english language this causes the data to be encoded as unicode hex
    16 bit characters.  For example:

    Phone sends "Test👍message", the MODEM sends
    "00540065007300740020D83DDC4D0020006D006500730073006100670065"

    For the human to read add spaces every 4th character on quad boundary:

    "0054 0065 0073 0074 0020 D83D DC4D 0020 006D 0065 0073 0073 0061 0067 0065"

    Looking at the same data a pattern emerges.

    0054 ascii T
    0065 ascii e
    0073 ascii s
    0074 ascii t
    0020 ascii ' ' space

    D83D unicode 👍 part-1
    DC4D unicode 👍 part-2

    So we can see that the data is simply unicode 16 bit in ascii hex.
    These type of payloads will always have quantities four characters
    ascii encoded with base 16 i.e. Ascii digits 0-9 and ascii letters A-F.

    This is super easy to covert.

    // assemble a 4 char string
    hex_ascii[0]= pdu[i];
    hex_ascii[1]= pdu[i+1];
    hex_ascii[2]= pdu[i+2];
    hex_ascii[3]= pdu[i+3];
    hex_ascii[4]= 0;

  	Then use the classic C function

    strtol(hex_ascii,0,16) to convert from base 16 ascii to an integer.

    //  base 16 convert 4 byte hex string to 16 bit integer
    unsigned long utf16 = strtol(hex_ascii,0,16);

	This converts 4 ascii character into a 16 bit decimal integer.


#endif



using namespace std;




std::string to_utf8(const wchar_t* buffer, int len)
{

/*
    std::string to_utf8(const wchar_t* buffer, int len)

    helper stream function for windows to convert and write windows
    wchar_t unicode characters to a stream so that data can written to
    log file opened as a classic stream

*/

	int nChars = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		NULL,
		0,
		NULL,
		NULL);
	if (nChars == 0) return "";

	string newbuffer;
	newbuffer.resize(nChars) ;
	::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		const_cast< char* >(newbuffer.c_str()),
		nChars,
		NULL,
		NULL);

	return newbuffer;
}

std::string to_utf8(const std::wstring& str)
{
/*
    std::string to_utf8(const std::wstring& str)

    helper stream function for windows to convert and write windows
    wchar_t unicode characters to a stream so that data can written to
    log file opened as a classic stream

*/

	return to_utf8(str.c_str(), (int)str.size());
}


boolean is_Ascii_Hex(unsigned char byte)
{

/*
    boolean is_Ascii_Hex(unsigned char byte)
    helper function to check if 8 bit character is ascii hex
    Digits 0-9 or letters A-F
*/


    unsigned char c = toupper(byte);

    if ( (c >= 'A') && (c <= 'F'))
        return true;
    else if ( (c >= '0') && (c <= '9'))
        return true;
    return false;
}

boolean is_Ascii_Hex_String(char *text)
{

/*
    boolean is_Ascii_Hex(unsigned char byte)

    helper function to scan every character in the text
    string. Returns true if every one of them contains
    ascii hex characters. Otherwise returns false.
*/


    int size = strlen(text);
    for (int i = 0; i < size; i++)
    {
		if (is_Ascii_Hex(text[i])==false)
            return false;
	}
    return true;
}

GsmPDU::GsmPDU()
{
    reset();
}

GsmPDU::~GsmPDU()
{


}

void GsmPDU::reset()
{
    size =0;
    size16 =0;
    as = "";
    us = "";
	memset(ud8,0,sizeof(ud8));
    memset(ud16,0,sizeof(ud16));
}

//	UTF_16
int GsmPDU::decode_UD(char *ud_text)
{

/*
    int GsmPDU::decode_UD(char *ud_text)

    The ud_text data can appear in two different formats.
    Unicode vs classic 8 bit ascii.  This decode function
    copies the data into two diffent result buffers

    unsigned char   ud8[1024];
    wchar_t		   	ud16[4096];


    One for 8 bit asii


    string. Returns true if every one of them contains
    ascii hex characters. Otherwise returns false.
*/




	reset();

    pdu = ud_text;      // make a copy of pdu ud

    // Is this text in old fashioned classic 8 bit ascii
    if (is_Ascii_Hex_String( (char*) pdu.c_str() )==false)
    {
        // Yes then no base 16 conversion required.
        // Text is can be read as is

        strcpy(ud8,pdu.c_str());
        size = pdu.size();
        us = ud_text;
        size16 = size;

        #if defined(Uses_GsmPDU_Debug)
        gsmout << "ud_text=" << ud_text << endl;
        gsmout << "as=" << us.c_str() << endl;
        #endif

        return size;
    }


    // We are here if data is encoded as base 16 ascii hex characters

    // convert local copy to upper case
    std::transform(pdu.begin(), pdu.end(), pdu.begin(), ::toupper);

    #if defined(Uses_GsmPDU_Debug)
    gsmout << "PDU1:" << pdu.c_str() << endl;
    gsmout << "PDU2:";
    #endif

    unsigned int size_last_word = pdu.size()-4; // calculate last quad boundary

    // Loop though ascii data processing 4 ascii characters at a time
    for (int i = 0; i <= size_last_word; i+=4)
    {

        #if defined(DOCUMENTATION)
        DGA 12/7/2023
        Phone sends "Test👍message", the MODEM sends
        "00540065007300740020D83DDC4D0020006D006500730073006100670065"

         For the human to read add spaces every 4th character on quad boundary:

        "0054 0065 0073 0074 0020 D83D DC4D 0020 006D 0065 0073 0073 0061 0067 0065"
        #endif

        char hex_ascii[16];

        // assemble a 4 char string
        hex_ascii[0]= pdu[i];       // fetch 1st char
        hex_ascii[1]= pdu[i+1];     // fetch 2nd char
        hex_ascii[2]= pdu[i+2];     // fetch 3rd char
        hex_ascii[3]= pdu[i+3];     // fetch 4th char
        hex_ascii[4]= 0;            // Null terminate


        // 	Use the classic C function strtol to convert
        //  base 16 4 byte hex string to 16 bit integer
		unsigned long utf16 = strtol(hex_ascii,0,16);  // DGA convert from ascii to decimal integer

        putc(utf16);  // Call helper function to process character into several buffers

    }

    ud8[size] = 0;      // Null terminate 8 bit copy
    ud16[size] = 0;     // Null terminate 16 bit unicode data.

    #if defined(Uses_GsmPDU_Debug)
    gsmout << endl;
    gsmout << ud8 << endl;
    #endif

    return size;
}

void GsmPDU::putc(unsigned short int uc16)
{
/*
    void GsmPDU::putc(unsigned short int uc16)

    Function to put character into various types of buffers
    for subsequent usage.


    unsigned char   ud8[1024];  // type-1, 8 bit classic ascii only
    wchar_t		   	ud16[4096]; // type-2 wchar_t windows format
	UnicodeString 	us;         // type-3 UnicodeString format
    AnsiString 		as;         // type-4 AnsiString format


    ud8 is only 8 bit printable ascii characters such as english text with no emoticons
    The ud8 container never has any 18 bit charcaters.  Only plain text. This is exactly
    the same as AT+CMGF=1 user text data when the sender has no unicode text in the sms.

    ud16 has all the data always formated as 16 bit wide unicode. This can be printed to screen
    or saved to a file under windows and it will look exactly the same as the sender's
    device,phone,computer used to send the message.

*/

	if (uc16 < 256)  // Is this an 8 bit ascii character
    {
        // Is there room in the local buffer ?
    	if (size < (sizeof(ud8)-1))
	    {
            ud8[size++] = uc16;  // store 8 bit only data
    	}
    }


    // Is there room in the local 16 bit buffer ?
    if (size16 < (sizeof(ud16)-1))
    {
        ud16[size16++] = uc16;   // store as 16 bit unicode
    }

    wchar_t w16 = uc16;     // copy and cast to Windows wchar_t
	us += w16;     			// save copy in UnicodeString us

    as += w16;


    #if defined(Uses_GsmPDU_Debug)
    gsmout << setfill('0') << setw(4) << hex << utf16;
    #endif


}



