Author id : 315147330

Personal details : 
Name : Amit Cohen
id : 315147330

Exercise name : HTTP proxy

Submitted files : 
1)proxy1.c : the code.
2)README : this file, which explains the code.


Remarks :   
1) I created a structure with 5 parameters, the name of the protocol, the path, the port, the host and the full path that includes the host.

In addition to the functions I received, I wrote 11 more functions:

1) finding_amount_of_characters. -->This function counts the number of occurrences of a particular character within a string.
2) check_protocol.-->The function checks if the URL protocol is HTTP.
3) URL_parsing.-->This function composes the structure of the URL, breaks down the address into path, host, port, protocol
4) get_index_of_third_slash.-->The function searches for the third slash and returns its index.
5) create_the_path.-->The function creates folders for the entire path and the latter produces as a file.
6) free_the_URL.-->The function releases the structure and all its features.
7) print_the_file_from_filesystem.-->The function opens the file according to the path obtained from the structure,. And prints the contents of the file.
8) connect_to_socket.-->Socket connection.
9) connect_to_server.-->Socket connection.
10) build_the_request.-->The function generates the HTTP request according to the fixed format.
11) show_the_page.-->The function writes the request to socket and reads what socket into the file we created and prints the extensions.
 


 
