# MFSCO
This tool can be used to determine a least-cost feedstock mix from crop residue, energy crop and MSW to meet conversion specifications, while also identifying appropriate depot locations and size, given that depot can be co-located with the biorefinery or can be located in any counties in the biofinery’s supply shed. Detail of the model’s assumption and input data can he found at https://www.sciencedirect.com/science/article/pii/S0306261919313479. The model was solved using IBM’s CPLEX Version 12.9 with C++ Concert Technology. It is assumed that the users have access to cplex library. When you compile a C++ application with a C++ library like CPLEX in Concert Technology, you need to tell your compiler where to find the CPLEX and Concert include files (that is, the header files), and you also need to tell the linker where to find the CPLEX and Concert libraries. An instruction can be found here (https://www.ibm.com/support/knowledgecenter/SSSA5P_20.1.0/ilog.odms.cplex.help/CPLEX/GettingStarted/topics/tutorials/Cplusplus/compiling.html To run this model with SQlite database, the user needs to have access to SQLite data base of BT16 resource assessment study.

