package bgu.spl171.net.impl.TFTP;

import java.io.IOException;
import java.util.concurrent.*;

import bgu.spl171.net.api.ConnectionHandler;
import bgu.spl171.net.api.bidi.Connections;


public class ConnectionsImpl<T> implements Connections<T>
{
	ConcurrentHashMap<Integer, ConnectionHandler<T>> handlerlist = new ConcurrentHashMap<>();
	ConcurrentHashMap<String, Integer > logedusers = new ConcurrentHashMap<>();
	int connectionId = 0;
	
	@Override
	public boolean send(int connectionId, T msg)
	{
		if(handlerlist.containsKey(connectionId))
		{
			handlerlist.get(connectionId).send(msg);
			return true;
		}
		return false;
	}

	public void broadcasttologed(T msg)
	{
		for(int i : logedusers.values())
			send(i, msg);
	}
	
	public void broadcast(T msg)
	{
		for (ConnectionHandler<T> i : handlerlist.values())
		{
			i.send(msg);
		} 	
	}

	@Override
	public void disconnect(int connectionId)
	{
		try{
		handlerlist.get(connectionId).close();
		handlerlist.remove(connectionId);
		}catch(IOException e){System.out.println("cannot disconnect user");}
	}
	
	public void addclient(Integer id, ConnectionHandler<T> handler)
	{
		handlerlist.put(id, handler);
	}
	
	public Integer logclient(String name, Integer id)
	{
		return logedusers.putIfAbsent(name, id);
	}
	
	public void removeclient(int connectionId)
	{
		logedusers.remove(connectionId);
		disconnect(connectionId);
	}
	
	public boolean search(int connectionId)
	{
		return handlerlist.containsKey(connectionId);
	}
	
	public int getId()
	{
		return connectionId;
	}
	
	public int getandIncId()
	{
		return connectionId++;
	}
}
