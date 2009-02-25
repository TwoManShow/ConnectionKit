/*
 Copyright (c) 2004-2006 Karelia Software. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, 
 are permitted provided that the following conditions are met:
 
 
 Redistributions of source code must retain the above copyright notice, this list 
 of conditions and the following disclaimer.
 
 Redistributions in binary form must reproduce the above copyright notice, this 
 list of conditions and the following disclaimer in the documentation and/or other 
 materials provided with the distribution.
 
 Neither the name of Karelia Software nor the names of its contributors may be used to 
 endorse or promote products derived from this software without specific prior 
 written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY 
 WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */
#import <Cocoa/Cocoa.h>

#define LocalizedStringInConnectionKitBundle(key, comment) \
[[NSBundle bundleForClass:[self class]] localizedStringForKey:(key) value:@"" table:nil]

@class CKTransferRecord;

// Some shared Error Codes

enum {
	ConnectionErrorUploading = 49101,
	ConnectionErrorDownloading,
	ConnectionErrorCreatingDirectory,
	ConnectionErrorChangingDirectory,
	ConnectionErrorDeleting,
	ConnectionErrorConnecting,
	ConnectionErrorDisconnecting,
	ConnectionErrorUnexpectedlyDisconnected,
	ConnectionErrorListingDirectory,
	ConnectionErrorGeneric,
};

@protocol CKConnection <NSObject, NSCopying>

+ (NSString *)name;

+ (id <CKConnection>)connectionToHost:(NSString *)host
                                 port:(NSNumber *)port
                             username:(NSString *)username
                             password:(NSString *)password
                                error:(NSError **)error;

+ (id <CKConnection>)connectionWithURL:(NSURL *)url error:(NSError **)error;

+ (id <CKConnection>)connectionWithName:(NSString *)name
                                   host:(NSString *)host
                                   port:(NSNumber *)port
                               username:(NSString *)username
                               password:(NSString *)password
                                  error:(NSError **)error;

/*!
 @method initWithHost:port:username:password:error:
 @abstract The designated initializer for connections.
 @discussion Initializes a connection object with the supplied parameters.
 @param host The host address to connect to.
 @param port The port to connect on. Supply nil to use the connection's default port, or if no port is required.
 @param username The username to use when connecting.
 @param password The password to use when connecting.
 @param error If an error occurs, upon return contains an NSError object that describes the problem.
 @result Returns an initialized connection object or nil if there was an error.
 */
- (id)initWithHost:(NSString *)host
			  port:(NSNumber *)port
		  username:(NSString *)username
		  password:(NSString *)password
			 error:(NSError **)error;

- (NSString *)host;
- (NSInteger)port;
- (NSString *)username;
- (NSString *)password;

- (void)setUsername:(NSString *)username;
- (void)setPassword:(NSString *)password;

// you can set a name on a connection to help with debugging
- (void)setName:(NSString *)name;
- (NSString *)name; 

- (void)setDelegate:(id)delegate;   // we do not retain the delegate
- (id)delegate;

- (void)connect;
- (BOOL)isConnected;
- (BOOL)isBusy;

/* disconnect queues a disconnection where as forceDisconnect '
   will terminate at the next available opportunity. */
- (void)disconnect;
- (void)forceDisconnect;
- (void)cleanupConnection;

- (void)changeToDirectory:(NSString *)dirPath;
- (NSString *)currentDirectory;

- (NSString *)rootDirectory;
- (void)createDirectory:(NSString *)dirPath;
- (void)createDirectory:(NSString *)dirPath permissions:(unsigned long)permissions;
- (void)setPermissions:(unsigned long)permissions forFile:(NSString *)path;

- (void)rename:(NSString *)fromPath to:(NSString *)toPath;
- (void)recursivelyRenameS3Directory:(NSString *)fromDirectoryPath to:(NSString *)toDirectoryPath;

- (void)deleteFile:(NSString *)path;
- (void)deleteDirectory:(NSString *)dirPath;
- (void)recursivelyDeleteDirectory:(NSString *)path;

- (void)startBulkCommands;
- (void)endBulkCommands;

- (void)uploadFile:(NSString *)localPath;
- (void)uploadFile:(NSString *)localPath toFile:(NSString *)remotePath;
- (void)uploadFile:(NSString *)localPath toFile:(NSString *)remotePath checkRemoteExistence:(BOOL)flag;
/* 
	New method that allows you to set a custom delegate for the upload.
	You must implement the ConnectionTransferDelegate informal protocol.
	By default the transfer record returned is the delegate of the transfer.
*/
- (CKTransferRecord *)uploadFile:(NSString *)localPath 
						  toFile:(NSString *)remotePath 
			checkRemoteExistence:(BOOL)flag 
						delegate:(id)delegate;
/* 
	returns CKTransferRecord as a heirarchy of what will be upload, remote and local files 
	can be found in the records node properties
*/
- (CKTransferRecord *)recursivelyUpload:(NSString *)localPath to:(NSString *)remotePath;
- (CKTransferRecord *)recursivelyUpload:(NSString *)localPath to:(NSString *)remotePath ignoreHiddenFiles:(BOOL)flag;

- (void)resumeUploadFile:(NSString *)localPath fileOffset:(unsigned long long)offset;
- (void)resumeUploadFile:(NSString *)localPath toFile:(NSString *)remotePath fileOffset:(unsigned long long)offset;

- (CKTransferRecord *)resumeUploadFile:(NSString *)localPath 
								toFile:(NSString *)remotePath 
							fileOffset:(unsigned long long)offset
							  delegate:(id)delegate;


- (void)uploadFromData:(NSData *)data toFile:(NSString *)remotePath;
- (void)uploadFromData:(NSData *)data toFile:(NSString *)remotePath checkRemoteExistence:(BOOL)flag;

/* 
	New method that allows you to set a custom delegate for the upload.
	You must implement the ConnectionTransferDelegate informal protocol.
	By default the transfer record returned is the delegate of the transfer.
*/
- (CKTransferRecord *)uploadFromData:(NSData *)data
							  toFile:(NSString *)remotePath 
				checkRemoteExistence:(BOOL)flag
							delegate:(id)delegate;

- (void)resumeUploadFromData:(NSData *)data toFile:(NSString *)remotePath fileOffset:(unsigned long long)offset;

- (CKTransferRecord *)resumeUploadFromData:(NSData *)data
									toFile:(NSString *)remotePath 
								fileOffset:(unsigned long long)offset
								  delegate:(id)delegate;

- (void)downloadFile:(NSString *)remotePath toDirectory:(NSString *)dirPath overwrite:(BOOL)flag;
- (void)resumeDownloadFile:(NSString *)remotePath toDirectory:(NSString *)dirPath fileOffset:(unsigned long long)offset;

/* 
	New method that allows you to set a custom delegate for the download.
	You must implement the CKConnectionTransferDelegate informal protocol.
	By default the transfer record returned is the delegate of the transfer.
*/
- (CKTransferRecord *)downloadFile:(NSString *)remotePath 
					   toDirectory:(NSString *)dirPath 
						 overwrite:(BOOL)flag
						  delegate:(id)delegate;

- (CKTransferRecord *)resumeDownloadFile:(NSString *)remotePath
							 toDirectory:(NSString *)dirPath
							  fileOffset:(unsigned long long)offset
								delegate:(id)delegate;

- (CKTransferRecord *)recursivelyDownload:(NSString *)remotePath
									   to:(NSString *)localPath
								overwrite:(BOOL)flag;

- (void)checkExistenceOfPath:(NSString *)path;

- (unsigned)numberOfTransfers;
- (void)cancelTransfer;
- (void)cancelAll;

- (void)directoryContents;
- (void)contentsOfDirectory:(NSString *)dirPath;

- (void)setProperty:(id)property forKey:(NSString *)key;
- (id)propertyForKey:(NSString *)key;
- (void)removePropertyForKey:(NSString *)key;

- (void)setTranscript:(NSTextStorage *)transcript;

- (double)uploadSpeed; // bytes per second
- (double)downloadSpeed;

- (NSString *)urlScheme; // by default calls class method
+ (NSString *)urlScheme; //eg http

- (void)editFile:(NSString *)remoteFile;

- (void)cancelTransfer;

@end



@interface NSObject (CKConnectionDelegate)

// There are 21 callbacks & flags.
// Need to keep NSObject Category, __flags list, setDelegate: updated

- (void)connection:(id <CKConnection>)con didChangeToDirectory:(NSString *)dirPath error:(NSError *)error;
- (BOOL)connection:(id <CKConnection>)con authorizeConnectionToHost:(NSString *)host message:(NSString *)message;
- (void)connection:(id <CKConnection>)con didConnectToHost:(NSString *)host error:(NSError *)error; // this only guarantees that the socket connected.
- (void)connection:(id <CKConnection>)con didAuthenticateToHost:(NSString *)host error:(NSError *)error; // this is a successful login
- (void)connectionDidSendBadPassword:(id <CKConnection>)con;

//SFTP Passphrase Support
- (NSString *)connection:(id <CKConnection>)con passphraseForHost:(NSString *)host username:(NSString *)username publicKeyPath:(NSString *)publicKeyPath;

- (void)connection:(id <CKConnection>)con didCreateDirectory:(NSString *)dirPath error:(NSError *)error;
- (void)connection:(id <CKConnection>)con didDeleteDirectory:(NSString *)dirPath error:(NSError *)error;
- (void)connection:(id <CKConnection>)con didDeleteFile:(NSString *)path error:(NSError *)error;


// recursivelyDeleteDirectory
//     These methods may change soon -- Seth
- (void)connection:(id <CKConnection>)con didDiscoverFilesToDelete:(NSArray *)contents inAncestorDirectory:(NSString *)ancestorDirPath;
- (void)connection:(id <CKConnection>)con didDiscoverFilesToDelete:(NSArray *)contents inDirectory:(NSString *)dirPath;
- (void)connection:(id <CKConnection>)con didDeleteDirectory:(NSString *)dirPath inAncestorDirectory:(NSString *)ancestorDirPath error:(NSError *)error;
- (void)connection:(id <CKConnection>)con didDeleteFile:(NSString *)path inAncestorDirectory:(NSString *)ancestorDirPath error:(NSError *)error;


- (void)connection:(id <CKConnection>)con didDisconnectFromHost:(NSString *)host;
- (void)connection:(id <CKConnection>)con didReceiveContents:(NSArray *)contents ofDirectory:(NSString *)dirPath error:(NSError *)error;
- (void)connection:(id <CKConnection>)con didReceiveContents:(NSArray *)contents ofDirectory:(NSString *)dirPath moreComing:(BOOL)flag;
- (void)connection:(id <CKConnection>)con didReceiveError:(NSError *)error;
- (void)connection:(id <CKConnection>)con didRename:(NSString *)fromPath to:(NSString *)toPath error:(NSError *)error;
- (void)connection:(id <CKConnection>)con didSetPermissionsForFile:(NSString *)path error:(NSError *)error;


- (void)connection:(id <CKConnection>)con download:(NSString *)path progressedTo:(NSNumber *)percent;
- (void)connection:(id <CKConnection>)con download:(NSString *)path receivedDataOfLength:(unsigned long long)length; 
- (void)connection:(id <CKConnection>)con downloadDidBegin:(NSString *)remotePath;
- (void)connection:(id <CKConnection>)con downloadDidFinish:(NSString *)remotePath error:(NSError *)error;


- (NSString *)connection:(id <CKConnection>)con needsAccountForUsername:(NSString *)username;
- (void)connection:(id <CKConnection>)con upload:(NSString *)remotePath progressedTo:(NSNumber *)percent;
- (void)connection:(id <CKConnection>)con upload:(NSString *)remotePath sentDataOfLength:(unsigned long long)length;
- (void)connection:(id <CKConnection>)con uploadDidBegin:(NSString *)remotePath;
- (void)connection:(id <CKConnection>)con uploadDidFinish:(NSString *)remotePath error:(NSError *)error;
- (void)connectionDidCancelTransfer:(id <CKConnection>)con; // this is deprecated. Use method below
- (void)connection:(id <CKConnection>)con didCancelTransfer:(NSString *)remotePath;

- (void)connection:(id <CKConnection>)con checkedExistenceOfPath:(NSString *)path pathExists:(BOOL)exists error:(NSError *)error;
@end

@interface NSObject (CKConnectionTransferDelegate)
- (void)transferDidBegin:(CKTransferRecord *)transfer;
- (void)transfer:(CKTransferRecord *)transfer transferredDataOfLength:(unsigned long long)length;
- (void)transfer:(CKTransferRecord *)transfer progressedTo:(NSNumber *)percent;
- (void)transfer:(CKTransferRecord *)transfer receivedError:(NSError *)error;
- (void)transferDidFinish:(CKTransferRecord *)transfer error:(NSError *)error;
@end

//registration type dictionary keys
extern NSString *ACTypeKey;
extern NSString *ACTypeValueKey;
extern NSString *ACPortTypeKey;
extern NSString *ACURLTypeKey; /* ftp://, http://, etc */

// Attributes for which there isn't a corresponding NSFileManager key
extern NSString *cxFilenameKey;
extern NSString *cxSymbolicLinkTargetKey;

//User Info Keys for Errors
extern NSString *ConnectionHostKey;
extern NSString *ConnectionDirectoryExistsKey;
extern NSString *ConnectionDirectoryExistsFilenameKey;

/*
 * The InputStream and OutputStream protocols, provides a transparent way to interchange
 * the implementation specific streams.
 */
@protocol InputStream <NSObject>
- (void)open;
- (void)close;
- (void)scheduleInRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)removeFromRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)setDelegate:(id)delegate;
- (id)delegate;
- (BOOL)setProperty:(id)property forKey:(NSString *)key;
- (id)propertyForKey:(NSString *)key;
- (NSError *)streamError;
- (NSStreamStatus)streamStatus;
- (BOOL)hasBytesAvailable;
- (int)read:(uint8_t *)buffer maxLength:(unsigned int)len;
@end

@protocol OutputStream <NSObject>
- (void)open;
- (void)close;
- (void)scheduleInRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)removeFromRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)setDelegate:(id)delegate;
- (id)delegate;
- (BOOL)setProperty:(id)property forKey:(NSString *)key;
- (id)propertyForKey:(NSString *)key;
- (NSError *)streamError;
- (NSStreamStatus)streamStatus;
- (BOOL) hasSpaceAvailable;

- (int)write:(const uint8_t *)buffer maxLength:(unsigned int)len;
@end