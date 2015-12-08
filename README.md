# CTalk
A simple peer-to-peer chat application using TCP in C

<h3>Index</h3>
<ol>
<li><a href="#requisites">Requisites</a></li>
<li><a href="#installation">Installation</a></li>
<li><a href="#usage">Usage</a></li>
<li><a href="#techused">Technology Used</a></li>
<li><a href="#testing">Testing</a></li>
</ol>

<a name="requisites"><h3>Requisites</h3></a>
<ul>
<li>Linux (Tested on Ubuntu-14.04)</li>
<li>Working gcc
	<ul>
	<li>To check gcc is working
		<ol>
		<li>Open a <b>terminal</b> window.</li>
		<li>Run "gcc".</li>
		<li>If it identifies the command, everything is perfect.</li>
		</ol>
	</li>
	</ul>
</li>
</ul>

<a name="installation"><h3>Installation</h3></a>
<ol>
<li>Download the files of this repository</li>
<li>Compile Instructions
	<ol>
	<li>gcc server.c -o server -lpthread</li>
	<li>gcc client.c -o client -lpthread</li>
	</ol>
</li>
</ol>

<a name="usage"><h3>Usage</h3></a>
<ol>
<li>Run one instance of server (<b>./server</b>)</li>
<li>Run multiple instances of client (<b>./client</b>)</li>
<li><i>wait</i> for connection in one client</li>
<li><i>conenct</i> to the above client from other client</li>
<li>Start chatting, send <i><b>end</b></i> to end the chat</li>
</ol>

<a name="techused"><h3>Technology Used</h3></a>
<ul>
<li><b>C</b> language</li>
</ul>

<a name="testing"><h3>Testing</h3></a>
The program has been tested on Ubuntu14.04, 64-bit

For Hugs and Bugs drop a mail at <b>asimkprasad@gmail.com</b>