package bgu.spl171.net.srv;

import bgu.spl171.net.api.MessageEncoderDecoder;
import bgu.spl171.net.api.MessagingProtocol;
import bgu.spl171.net.api.bidi.BidiMessageEncoderDecoder;
import bgu.spl171.net.api.bidi.BidiMessagingProtocol;
import bgu.spl171.net.impl.TFTP.ConnectionsImpl;
import bgu.spl171.net.api.ConnectionHandler;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final BidiMessagingProtocol<T> protocol;
    private final BidiMessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private int connectionId;
    private ConnectionsImpl<T> connections;

    public BlockingConnectionHandler(Socket sock, BidiMessageEncoderDecoder<T> reader, BidiMessagingProtocol<T> protocol, ConnectionsImpl<T> conn, int id) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections = conn;
        this.connectionId = id;
    }

    @Override
    public void run() {
    	protocol.start(connectionId, connections);
    	connections.addclient(connectionId++, this); 
    	
        try (Socket sock = this.sock)
        { //just for automatic closing
            int read;
            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());
            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0)
            {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null)
                {
                	protocol.process(nextMessage);
                }
            }

          } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

	@Override
	public void send(T msg)
	{
	     if(msg != null)
         {
            try
            {
            	out.write(encdec.encode(msg));
            	out.flush();  	
            }catch(IOException e){System.out.println(e);}
         }
	}
}
