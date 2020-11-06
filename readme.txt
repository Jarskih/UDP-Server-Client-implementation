Jari Hanski

Features:

Did not have time for sounds and effects :(

Server discovery
- Using 192.168.0.255 broadcast address (You can change it if needed in config.h)

Reliable messages (reliable_events.h & charlie_gameplay.hpp)
- I did not use acknowledge as message id because it was buggy. Therefore the id(index_) is contained in ReliableMessageQueue.

Pass w/ Distinction:

Map downloading from server if not found on client. Does not save the map on file only to memory. 
- I did not figure out how to send out map (did not fit in one package?) in less packages so now it will send every tile as separate package.

Cut bandwidth usage
- Originally I planned to send updates for projectiles but since you mentioned it is not needed the projectiles are simulated on client.
- Sending entity and player data has been optimized by sending less data.
	- Position as uint16 for x and y
	- Turret and tank rotation as int16;
	- Similarly input bits use int16 for rotation (istead of float)

Master server
- master server address is in /assets/masterserver.txt
- Local game server sends heartbeat messages to keep them in the server list
- NodeJS master server keeps a list of local  server addresses
- Clients requests game server from master server and receives it as bytearray. // Not finished the network reader reads the package before it arrives to master_server_client but should work after small refactor
- Needs nodejs installed
- npm install
- node index.js


Assets:
https://free-game-assets.itch.io/free-2d-tank-game-assets
https://2dgameartguru.com/top-down-extras-2-tank/
https://adamatomic.itch.io/gallet-city
https://pixabay.com/photos/tank-war-military-army-battle-4885714/
https://opengameart.org/node/46538