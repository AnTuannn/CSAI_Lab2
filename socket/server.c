/* 1.Tạo các #include cần thiết */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
/* dành riêng cho AF_INET */
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	/* 2. Thực hiện khởi tạo socket mới cho trình chủ */
	server_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	/* 3. Đặt tên và gán địa chỉ kết nối cho socket theo giao thức Internet */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	server_address.sin_port = htons( 9734 );
	server_len = sizeof( server_address );
	
	/* 4. Ràng buộc tên với socket */
	bind( server_sockfd, (struct sockaddr *)&server_address, server_len );
	
	/* 5. Mở hàng đợi nhận kết nối - cho phép đặt hàng vào hàng đợi tối đa 5 kết nối */
	listen( server_sockfd, 5 );
	
	/* 6. Lặp vĩnh viễn để chờ và xử lý kết nối của trình khách */
	while ( 1 ) {
		char ch;
		printf( "server waiting...\n" );
		/* Chờ và chấp nhận kết nối */
		client_sockfd = accept( server_sockfd, (struct sockaddr*)&client_address, &client_len );
		/* Đọc dữ liệu do trình khách gửi đến */
		read( client_sockfd, &ch, 1 );
		ch++;
		/* Gửi trả dữ liệu về cho trình khách */
		write( client_sockfd, &ch,1 );
		/* Đóng kết nối */
		close( client_sockfd );
	}
}