Jari Hanski

Updates for supplementation 3:
- Added .env file for master server for changing port and IP address
- Server stores inputs from clients and applies the correct input based on tick
- Added tick field to NetworkMessageInputCommand from client

Updates for supplementation 2:
- Fixed Inputinator::update_predicted_position method
- Created standalone nodejs app for master server (masterserver/index.exe)
- Switched using tickrate instead of dt in update
- Fixed Interpolator
- Changed unsigned ints to signed ints
- Master server client receives game server IP properly
- Server discovery using broadcast address switched to using master server

Features:

Server discovery
- Using 192.168.0.255 broadcast address (You can change it if needed in config.h)

Reliable messages (reliable_events.h & charlie_gameplay.hpp)
- I did not use acknowledge as message id because it was buggy. Therefore the id(index_) is contained in ReliableMessageQueue.

Pass w/ Distinction:

Map downloading 
- Client downloads the map from the server if not found on the client. Note: does not save the map on file. 
- I did not figure out how to send out map (did not fit in one package?) in less packages so now it will send every tile as separate package.
- You can test this by deleting (assets/map1.txt) from client


Cut bandwidth usage
- Sending entity and player data has been optimized by sending less data.
	- Position as uint16 for x and y
	- Turret and tank rotation as int16;
	- Similarly input bits use int16 for rotation (instead of float)

Master server
- master server address is in /assets/masterserver.txt
- You can change node app port and IP address in the .env file
- Local game server sends heartbeat messages to keep them in the server list
- NodeJS master server keeps a list of local  server addresses
- Clients requests game server from master server and receives it as bytearray
- launch masterserver from: "/masterserver/index.exe"


Assets:
https://free-game-assets.itch.io/free-2d-tank-game-assets
https://2dgameartguru.com/top-down-extras-2-tank/
https://adamatomic.itch.io/gallet-city
https://pixabay.com/photos/tank-war-military-army-battle-4885714/
https://opengameart.org/node/46538