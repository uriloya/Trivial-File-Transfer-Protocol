package bgu.spl171.net.srv;

import java.util.function.Supplier;

import bgu.spl171.net.api.bidi.BidiMessageEncoderDecoder;
import bgu.spl171.net.api.bidi.BidiMessagingProtocol;

public class ThreadPerClientServer<T> extends BaseServer<T> 
{
	
    public ThreadPerClientServer(
            int port,
            Supplier<BidiMessagingProtocol<T>> protocolFactory,
            Supplier<BidiMessageEncoderDecoder<T>> encoderDecoderFactory) {
 
        super(port, protocolFactory, encoderDecoderFactory);
    }
    
	@Override
	protected void execute(BlockingConnectionHandler handler)
	{
		new Thread(handler).start();
	}
}
