package net.wordpuzzle.wasi;

import java.io.Closeable;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

// https://stackoverflow.com/a/1244456/907773

/**
 * @author rjzak
 */
public class FDSocket implements Closeable {
    protected int fd;
    protected FileInputStream is;
    protected FileOutputStream os;

    public FDSocket(int fdnum) {
        this.fd = fdnum;

        Class<FileDescriptor> clazz = FileDescriptor.class;

        Constructor<FileDescriptor> c;
        try {
            c = clazz.getDeclaredConstructor(new Class[] { Integer.TYPE });
        } catch (SecurityException | NoSuchMethodException e) {
            e.printStackTrace();
            return;
        }

        FileDescriptor fd;
        try {
            fd = c.newInstance(Integer.valueOf(fdnum));
        } catch (IllegalArgumentException | InstantiationException | IllegalAccessException |
                 InvocationTargetException e) {
            e.printStackTrace();
            return;
        }

        c.setAccessible(true);
        os = new FileOutputStream(fd);
        is = new FileInputStream(fd);
    }

    public int getFD() {
        return fd;
    }

    @Override
    public void close() throws IOException {
        os.close();
        is.close();
    }

    public InputStream getInputStream() {
        return is;
    }

    public OutputStream getOutputStream() {
        return os;
    }
}
