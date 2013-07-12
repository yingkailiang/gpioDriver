#include <stdio.h>
#include <fcntl.h>

#define DATA_LEN 1

int main(void)
{
	int fd;
	char gpio_buffer[10];
	char choice[10];
 
	fd = open( "/dev/gpio_drv", O_RDWR );
 
	printf( "Value of fd is: %d", fd );
 
	if( fd < 0 )
	{
		printf("Cannot open device \t");
		return 0;
	}
 

	while(1)
	{
		printf("\nPlease enter choice: \t");
		scanf( "%s", choice );
		printf("Your choice is: %s \n", choice );
		write( fd, choice,DATA_LEN );
		read( fd, gpio_buffer, DATA_LEN);
		printf("GPIO value is: %s \n", gpio_buffer );
 	}


	if( 0 != close(fd) )
	{
		printf("Could not close device\n");
	}
 
	return 0;
}
