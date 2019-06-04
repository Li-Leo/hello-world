Step 1: Check for SSH Keys

$ ls -al ~/.ssh
# Lists all the files in your .ssh directory, if they exist
Generating public/private rsa key pair

Step 2: Generate a new SSH key
$ ssh-keygen -t rsa -b 4096 -C "mcflym@N123456"
$ ssh-keygen -t rsa -C "mcflym@N123456"
# Creates a new ssh key, using the provided domain username and computer name as a label
Generating public/private rsa key pair.

Step 3: Add your key to the ssh-agent
To configure the ssh-agent program to use your SSH key, first ensure ssh-agent is enabled

# start the ssh-agent in the background
$ eval $(ssh-agent -s)
Agent pid 59566

If you are using Git Bash, turn on the ssh-agent with command shown below instead:   ssh-copy-id
# start the ssh-agent in the background
$ eval `ssh-agent`
Agent pid 59566

Then, add your SSH key to the ssh-agent:
$ ssh-add ~/.ssh/id_rsa

Step 4: Add your SSH key to the server
$ cat ~/.ssh/id_rsa.pub | ssh username@server.address.com 'cat >> ~/.ssh/authorized_keys'