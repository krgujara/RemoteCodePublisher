
Remote Code Publisher

Message Passing System of Remote Code Publisher - CPP_Projects/RemoteCodePublisher/RemoteCodePublisher/MsgPassingActivity.pdf

Instructions to Run this project - 

All the functionality is demonstrated by sending and receiving files
, msgs, from client to server and vice versa


FUNCTIONILTY- 
1. Uploading Files to Server, as per the Categories provided by client
2. Publishing all the uploadded files in the form of html files
3. On Request,Downloading files from the server
Downloads is the folder where all downloaded files are stored
4. I did set Up IIS, and all the server files go to that folder.
5. Delete requested file from server (Some times, it says that file to be deleted not found,
 because, Since it is working in the multithreaded enviromnet, file
CLient is requesting to delete the file before it is been created)
6. Server is able to on request able to send all file names to client
 in a specified category.

For setting up IIS, You need to set the path as the command line argumt
to server side, and then it automatically does the rest.


I have tried my level best create the GUI, tried to debug the code, check why GUI
is not working, while debugging, I could see that msgs are received 
to Server, but Couldnt figure out why it keeps looping to connect to server.

I have tried my level best to make it run, I am Using 2 queues to communicate 
to the client Package (SendQueue and Receive Queue).


When ever client wants to send some message, he enqueues msgs in that
queue and when ever he wants msgs, he dequeues,

I am calling all the functins to client on seperate thread, not 
on the same GUI thread, Hence that again is not the issue.

Because of which I couldnot set GUI as a startup project since I wanted
to demonstrate all requirements, and for which I should have made client 
as a static library, then I wouldnot have been able to demonstrate requirements,
So I did demonstrate the requiremnts on client and server console.

Also since I have tried GUI, Please Consider my Bonus Question Also

  
