# Security Model

To test the proposed security model we need to use 4 datasets:

	"normal.txt" -> to generate valid detectors
	"train.txt" -> training phase
	"test.txt" -> test phase
	"alert_data.txt" -> source and destination from each traffic pattern on test dataset

The commands to run the proposed system:

	./MAIS-IDS_Client.out number-of-detectors number-of-instances

	./MAIS-IDS_Server.out number-of-detectors number-of-instances

Example:

	If we need two client instances and one Orchestra instance with 15 detectors on each client instance:

		1- First we set the following commands on each Linux terminal:	

		./MAIS-IDS_Server.out 15 2 -> Orchestra
		./MAIS-IDS_Client.out 15 2 -> Client
		./MAIS-IDS_Client.out 15 2 -> Client



