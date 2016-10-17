# OICNSIM
Overlay ICN (O-ICN) Simulator
-------------------------------

OICNSIM is a simulator to simulate Information Centric Networking (ICN) based network using NS3. OICNSIM is based on 
Overlay ICN (O-ICN) architecture proposed in the literature. O-ICN architecture is an overlay architecture which is backward compatible and can coexist with the current Internet. The data exchange in this architectrue can happen though the traditional routers as well which enables O-ICN to be incrementally deployable.
 
Table of Contents:
------------------

1) Prerequiste
2) Building OICNSIM
3) Running OICNSIM

1) Prerequiste :
---------------
	1.1) ns3 (ver3.24) must be on installed on your system.

2) Building OICNSIM :
--------------------
	2.1) Clone the OICNSIM repository to your local system as follows:
		i) Clone the repository in your home folder using 
			"git clone https://github.com/samarshailendra/OICNSIM.git"
		ii) Create "o-icn" folder under the ~ns3.24/src/
		iii) Move the contents of OICNSIM folder to o-icn folder.		
		

	2.2) From the root folder of ns-3, run the following to clean any previous builds.
		./waf clean

	2.3) Set the flags following flags to enabled C++11 and not to treat warnings as erros
		export CFLAGS='-Wall -Wextra -std=c11 -std=gnu++11 -Wno-error'
		export CPPFLAGS='-std=c++11 -std=gnu++11 -Wno-error'
		export CXXFLAGS='-std=c++11 -std=gnu++11 -Wno-error'

	2.4) Run the following to configure ns-3	
		./waf configure --enable-sudo

	2.5) Rebuild ns3.
		./waf build

3) Running OICNSIM:
------------------
	3.1) Copy the oicn-example.cc from src/o-icn/example to scratch folder. 
		cp <ns-root-folder>/src/o-icn/example <ns-root-folder>/scratch

	3.2) Execute the following to run the example. 
		./waf --run oicn-example 
