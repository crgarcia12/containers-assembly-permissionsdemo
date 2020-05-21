# Why not runnng docker as root?

In here we will denmostrate why you should not create docker containers as root

### First let's create a file that only root can access:
```
sudo -s
cd /root
echo "top secret stuff" >> ./secrets.txt 
chmod 0600 secrets.txt
ls -l
cat secrets.txt
exit
cat /root/secrets.txt
```

### Now we run this container
```
docker build . -t spycontainer
docker run -v /root/secrets.txt:/tmp/secrets.txt spycontainer
```

Even further:
```
docker run -v /etc/shadow:/tmp/secrets.txt spycontainer
```

You can now check what would happen if you would run as a non-root user:
```
id
docker run --user 1000 -v /etc/shadow:/tmp/secrets.txt spycontainer
```