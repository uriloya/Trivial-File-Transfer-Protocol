package bgu.spl171.net.impl.TFTP;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedDeque;

import bgu.spl171.net.api.bidi.BidiMessagingProtocol;
import bgu.spl171.net.api.bidi.Connections;
import bgu.spl171.net.impl.Packets.ACKPacket;
import bgu.spl171.net.impl.Packets.BCASTPacket;
import bgu.spl171.net.impl.Packets.DATAPacket;
import bgu.spl171.net.impl.Packets.DELRQPacket;
import bgu.spl171.net.impl.Packets.DIRQPacket;
import bgu.spl171.net.impl.Packets.DISCPacket;
import bgu.spl171.net.impl.Packets.ERRORPacket;
import bgu.spl171.net.impl.Packets.LOGRQPacket;
import bgu.spl171.net.impl.Packets.Packet;
import bgu.spl171.net.impl.Packets.RWRQPacket;
import bgu.spl171.net.api.ConnectionHandler;
import java.nio.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class TFTPProtocol implements BidiMessagingProtocol<Packet>
{
	ConnectionsImpl<Packet> connections;
	int clientid;
	boolean shouldterminate = false;
	String currentfile;
	File file;
	ConcurrentHashMap<Integer, byte[]> dataqueue = new ConcurrentHashMap<>();
	ConcurrentHashMap<Integer, byte[]> allblocks = new ConcurrentHashMap<>();
	static ConcurrentHashMap<String, Integer> files = new ConcurrentHashMap<>();
	
	public TFTPProtocol()
	{
		File dir = new File("Files/");
	    if(dir.isDirectory())
	    {
	        for(File tmp:dir.listFiles())
	        {
	            if(tmp.isFile())
	            {
	            	files.put(tmp.getName(), 1);
	            }
	        }
	    }
	}
	
	@Override
	public void start(int connectionId, Connections<Packet> connections)
	{
		clientid = connectionId;
		this.connections = (ConnectionsImpl<Packet>) connections;
		currentfile = "";		
	}

	@Override
	public void process(Packet message)
	{
		Packet plogerr;
		short op = message.getOpcode();
		switch (op)
		{
			case 1: //Read request    
				if(connections.logedusers.contains(clientid))
					readfunc((RWRQPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 2: //Write request
				if(connections.logedusers.contains(clientid))
					writefunc((RWRQPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 3: //Data request 
				if(connections.logedusers.contains(clientid))
					datafunc((DATAPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
					dataqueue.clear();
					allblocks.clear();
				}
				break;
			case 4: //Acknowledgment request	
				if(connections.logedusers.contains(clientid))
					ackfunc((ACKPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 5: //Error
				if(connections.logedusers.contains(clientid))
					errorfunc((ERRORPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 6: //Directory listing request			
				if(connections.logedusers.contains(clientid))
					dirfunc((DIRQPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 7: //Login request 
				if(connections.logedusers.contains(clientid))
				{
					plogerr = new ERRORPacket((short)5, (short)7, "User already logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				else
					logfunc((LOGRQPacket) message);
				break;
			case 8: //Delete request	
				if(connections.logedusers.contains(clientid))
					delfunc((DELRQPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 9: //Broadcast file added/deleted  			
				if(connections.logedusers.contains(clientid))
					bcastfunc((BCASTPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			case 10: //Disconnect
				if(connections.logedusers.contains(clientid))
					discfunc((DISCPacket) message);
				else
				{
					plogerr = new ERRORPacket((short)5, (short)6, "User not logged in" +'\0');
					connections.send(clientid, plogerr);
				}
				break;
			default:
				Packet p = new ERRORPacket((short)5, (short)4, "Illegal TFTP operation " +'\0'); 
				connections.send(clientid, p);
				break;	
		}
	}
	
	private void logfunc(LOGRQPacket message)
	{
		if(connections.logclient(message.getUserName(), clientid) == null)
		{
			ACKPacket p = new ACKPacket((short)4, (short)0);
			connections.send(clientid, p);
		}
		else
		{
			Packet p = new ERRORPacket((short)5, (short)7, "User already logged in" +'\0');
			connections.send(clientid, p);
		}
	}
	
	private void delfunc(DELRQPacket message)
	{	
		currentfile = message.getFileName().substring(0, message.getFileName().length()-1);
		if(!files.containsKey(currentfile))
		{
			Packet p = new ERRORPacket((short)5, (short)1, "File not found" +'\0');
			connections.send(clientid, p);
		}
		else
		{
			try
			{
				files.remove(currentfile);
				Path path = Paths.get("Files/" + currentfile);
				Files.delete(path);
			}catch(IOException e)
			{Packet p = new ERRORPacket((short)5, (short)2, "Access violation" +'\0');
			connections.send(clientid, p);}
			
			Packet p = new ACKPacket((short)4, (short)0);
			connections.send(clientid, p);
			Packet msg = new BCASTPacket((short)9, (short)0, currentfile +'\0');
			bcastfunc((BCASTPacket)msg);
			currentfile = "";
		}
	}
	
	private void readfunc(RWRQPacket message)
	{	
		currentfile = message.getFileName().substring(0, message.getFileName().length()-1);
		if(!files.containsKey(currentfile) || files.get(currentfile).intValue() == 0)
		{
			Packet p = new ERRORPacket((short)5, (short)1, "File not found" +'\0');
			connections.send(clientid, p);
		}
		else
		{		
			try
			{	
				Path path = Paths.get("Files/" + currentfile);
				byte[] bytes = Files.readAllBytes(path);
				allblocks = splitarray(bytes);
				DATAPacket p = new DATAPacket((short)3, (short)allblocks.get(1).length,(short)1, allblocks.get(1));
				connections.send(clientid, p);
			}
			catch (FileNotFoundException e)
			{
				Packet p = new ERRORPacket((short)5, (short)1, "File not found" +'\0');
				connections.send(clientid, p);
			}
			catch(IOException e)
			{
				Packet p = new ERRORPacket((short)5, (short)2, "Access violation" +'\0');
				connections.send(clientid, p);
			}
		}	
	}
	
	private void writefunc(RWRQPacket message)
	{
		currentfile = message.getFileName().substring(0, message.getFileName().length()-1);
		if(!files.containsKey(currentfile))
		{
			files.put(currentfile, 0);
			file = new File("Files/" + currentfile);
			Packet ackp = new ACKPacket((short)4, (short)0);
			connections.send(clientid, ackp);
		}
		else
		{
			currentfile = "";
			Packet p = new ERRORPacket((short)5, (short)5, "File already exists" +'\0');
			connections.send(clientid, p);
		}
	}
	
	private void dirfunc(DIRQPacket message)
	{
		String allfilesname = "";
		Iterator<Map.Entry<String, Integer>> it = files.entrySet().iterator();
		while (it.hasNext()) {
		    Map.Entry<String, Integer> pair = it.next();
		    allfilesname += pair.getKey() + '\n';
		}
		allfilesname.substring(0, allfilesname.length()-1);
		byte [] namesinbyte = allfilesname.getBytes();
		allblocks = splitarray(namesinbyte);
		DATAPacket p = new DATAPacket((short)3, (short)allblocks.get(1).length,(short)1, allblocks.get(1));
		connections.send(clientid, p);
	}
	
	private void datafunc(DATAPacket message)
	{
		if(message.getSize() == 512)
		{
			dataqueue.put((int)message.getBlock(), message.getData());
			Packet ackp = new ACKPacket((short)4, message.getBlock());
			connections.send(clientid, ackp);
		}
		else
		{
			if(message.getBlock() == dataqueue.size()+1)
			{
				dataqueue.put((int)message.getBlock(), message.getData());
				Packet ackp = new ACKPacket((short)4, message.getBlock());
				connections.send(clientid, ackp);
				byte[] buffer = new byte[(dataqueue.size()-1)*512 + message.getSize()];
						
				int i = 0;
				for (int j = 1; j <= message.getBlock(); j++)
				{
					byte[] temp = dataqueue.remove(j);
					for (int k = 0; k < temp.length; k++)
						buffer[i++] = temp[k];
				}
				
				try
				{
					FileOutputStream out = new FileOutputStream(file);
					out.write(buffer);
					out.close();
					files.replace(currentfile, 1);
				}catch (IOException e) 
				{Packet p = new ERRORPacket((short)5, (short)2, "Access violation" +'\0');
				connections.send(clientid, p);}
				Packet msg = new BCASTPacket((short)9, (short)1, currentfile +'\0');
				bcastfunc((BCASTPacket)msg);
				currentfile = "";
			}
			else
			{
				Packet p = new ERRORPacket((short)5, (short)0, "Not defined" +'\0');
				connections.send(clientid, p);
				dataqueue.clear();
			}
		}
	}
	
	private void ackfunc(ACKPacket message)
	{
		if(allblocks.size() > message.getBlock())
		{
			DATAPacket p;
			p = new DATAPacket((short)3, (short)allblocks.get(message.getBlock()+1).length,(short)(message.getBlock()+1), allblocks.get(message.getBlock()+1));
			connections.send(clientid, p);
		}
		else if(message.getBlock() > 0 && allblocks.size() == message.getBlock())
		{
			allblocks.clear();
		}
		else
		{
			allblocks.clear();
			currentfile = "";
		}
	}
	
	private void bcastfunc(BCASTPacket message)
	{
		connections.broadcasttologed(message);
	}
	
	private void errorfunc(ERRORPacket message)
	{
		dataqueue.clear();
		allblocks.clear();
	}
	
	private void discfunc(DISCPacket message)
	{
		Packet ackp = new ACKPacket((short)4, (short)0);
		connections.send(clientid, ackp);
		connections.removeclient(clientid);
	}

	@Override
	public boolean shouldTerminate() 
	{
		return shouldterminate;
	}
	
	ConcurrentHashMap<Integer, byte[]> splitarray(byte[] allbytes)
	{
		int x = 512;
		int len = allbytes.length;
		int counter = 1;
		ConcurrentHashMap<Integer, byte[]> all = new ConcurrentHashMap<>();
		for (int i = 0; i < len - x + 1; i += x)
			all.put(counter++, (Arrays.copyOfRange(allbytes, i, i + x)));
		if (len % x != 0)
			all.put(counter, (Arrays.copyOfRange(allbytes, len - len % x, len)));
		return all;
	}
}














