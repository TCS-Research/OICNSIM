# OICNSIM
Overlay ICN (O-ICN) Simulator
-------------------------------

OICNSIM is a simulator to simulate  Overlay Information Centric Networking (O-ICN) [1] based network using ns3. It is backward compatible and can coexist with the current Internet. The data exchange in this architectrue can take place though the legacy routers as well ICN routers, which makes O-ICN both backward compatible and incrementally deployable.
 
Table of Contents:
------------------

1) Prerequiste
2) Building OICNSIM
3) Running OICNSIM exmaple

1) Prerequiste:
---------------
	1.1) ns3 must be on installed on your system. Tested with ns3.24 on Ubuntu 14.04.
	1.2) Boost libraries must be installed.	

2) Building OICNSIM:
--------------------
	2.1) Clone the OICNSIM repository to your local system as follows:
		i) Clone the repository in your home folder using 
			"git clone https://github.com/TCS-Research/OICNSIM.git"
		ii) Create "o-icn" folder under the ~ns3.24/src/
		iii) Move the contents of <HOME>/OICNSIM/ folder to ~/ns3.24/src/o-icn/ folder.		
		

	2.2) From the root folder of ns3, run the following to clean any previous builds.
		./waf clean

	2.3) Set the flags following flags to enabled C++11 and not to treat warnings as erros.
		export CFLAGS='-Wall -Wextra -std=c11 -std=gnu++11 -Wno-error'
		export CPPFLAGS='-std=c++11 -std=gnu++11 -Wno-error'
		export CXXFLAGS='-std=c++11 -std=gnu++11 -Wno-error'

	2.4) Run the following to configure ns3	
		./waf configure --enable-sudo

	2.5) Rebuild ns3.
		./waf build

3) Running OICNSIM exmaple:
---------------------------
	3.1) Copy the oicn-example.cc from src/o-icn/example to scratch folder. 
		cp <ns-root-folder>/src/o-icn/example <ns3-root-folder>/scratch

	3.2) Execute the following to run the example. 
		./waf --run oicn-example 


Devloped by: TCS Research & Innovation
-----------------------------------------------------------------
Please do remember to cite us !

[1] S. Shailendra, B. Panigrahi, H. K. Rath, and A. Simha, “A Novel Overlay Architecture for Information Centric Networking,” in Twenty First IEEE National Conference on Communications (NCC), pp. 1–6, 2015.

[2] Suvrat Agrawal, Samar Shailendra, Anirudh Morabagal, Bighnaraj Panigrahi, Hemant Kumar Rath, Anantha Simha, “Overlay ICN Simulator (OICNSIM),” https://github.com/TCS-Research/OICNSIM.

