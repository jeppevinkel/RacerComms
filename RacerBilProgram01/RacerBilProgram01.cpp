

int main(int argc, char **args)
{
	btSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.addressFamily = AF_BTH;
	sockAddr.serviceClassId = RFCOMM_PROTOCOL_UUID;
	sockAddr.port = BT_PORT_ANY;
	sockAddr.btAddr = 9006280871e4;
	error = connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	return 0;
}


// This just redundant!!!!

//

// Convert a formatted BTH address string to populate a BTH_ADDR (actually a ULONGLONG)

ULONG AddrStringToBtAddr(const char * pszRemoteAddr, BTH_ADDR * pRemoteBtAddr)

{

	ULONG ulAddrData[6], ulRetCode = 0;

	BTH_ADDR BtAddrTemp = 0;

	int i;



	if ((pszRemoteAddr == NULL) || (pRemoteBtAddr == NULL))

	{

		ulRetCode = 1;

		goto CleanupAndExit;

	}



	*pRemoteBtAddr = 0;



	// Populate a 6 membered array of unsigned long integers

	// by parsing the given address in string format

	//sscanf_s(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2],

		//&ulAddrData[3], &ulAddrData[4], &ulAddrData[5]);



	// Construct a BTH_ADDR from 6 integers stored in the array

	for (i = 0; i < 6; i++)

	{

		// Extract data from the first 8 lower bits.

		BtAddrTemp = (BTH_ADDR)(ulAddrData[i] & 0xFF);

		// Push 8 bits to the left

		*pRemoteBtAddr = ((*pRemoteBtAddr) << 8) + BtAddrTemp;

	}



CleanupAndExit:

	return ulRetCode;

}