# GSM-PDU
C++class for GSM MODEM to decode SMS text message PDU/UD data into both ascii and unicode 

December 11,2023 

GSM PDU.h 
GSM PDU.cpp

C++ Class example to teach how decode PDU user data from a Multitech modem  

Greetings from St Louis, Missouri USA

I'm a C/C++ software engineer.  I deveoped an SMS gateway server app for my employer AMCOM. 

When using a GSM modems in text mode i.e. AT+CMGF=1 the user data can appear in two different formats. Unicode vs classic 8 bit ascii.

On the internet there are myths that have been perpetuated about how this data is encoded making it very confusing and complicated.  Maybe this was true 10-15 years ago.  But today modems with telbit modules the user data is simply in two posisble formats as follows. 


if the SMS message includes any unicode 16 bit chracters such as emoticons or any non english language this causes the data to be encoded as unicode hex 16 bit characters.  For example:

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


Hope this class is usefull.

Best regards,

Dan Ambrose
C/C++ Software Engineer
Saint Louis, MO USA
email: dan27125@gmail.com

