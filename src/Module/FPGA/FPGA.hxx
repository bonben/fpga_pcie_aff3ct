#include <string>

#include <cstring>
#include <sstream>
#include <stdlib.h>

#include <poll.h> // C lib for polling event
#include <fcntl.h> // C lib for various function including "open" function

#include "Tools/Exception/exception.hpp"
#include "Tools/Exception/exception_fpga.hpp"
#include "Module/FPGA/FPGA.hpp"
#include "Tools/fpga_utils.hpp"

namespace aff3ct
{
namespace module
{

template <typename D>
Task& FPGA<D>
::operator[](const fpg::tsk t)
{
	return Module::operator[]((size_t)t);
}

template <typename D>
Socket& FPGA<D>
::operator[](const fpg::sck::send s)
{
	return Module::operator[]((size_t)fpg::tsk::send)[(size_t)s];
}

template <typename D>
Socket& FPGA<D>
::operator[](const fpg::sck::receive s)
{
	return Module::operator[]((size_t)fpg::tsk::receive)[(size_t)s];
}

template <typename D>
FPGA<D>
::FPGA(const int N, const int n_frames)
: Module(n_frames), N(N)
{
	const std::string name = "FPGA";
	this->set_name(name);
	this->set_short_name(name);

	const char* fwrite = "/dev/xdma0_h2c_0";
	const char* fread = "/dev/xdma0_c2h_0";

	/*
	Opening the write device file, so we don't need to reopen it everytime. Reopening this file for every write
	causes huge performance loss.
	*/
	this->fs_write = fopen(fwrite, "wb");
	if (this->fs_write == NULL)
	{
		std::stringstream message;
		message << "Error opening " << fwrite << " : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	/*
	Opening the read device file, so we don't need to reopen it everytime. Reopening this file for every read
	causes huge performance loss.
	*/
	this->fs_read = fopen(fread, "rb");
	if (this->fs_read == NULL)
	{
		std::stringstream message;
		message << "Error opening " << fread << " : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	// We also compute how many descriptor we need and some variables so we don't need to compute everytime
	this->buffer_count = ((this->N * this->n_frames * sizeof(D)- 1)/4096) + 1;

	if (N <= 0)
	{
		std::stringstream message;
		message << "'N' has to be greater than 0 ('N' = " << N << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}
	// Allocate memory buffers. MUST be a multiple of 4096 (DMA specs)
	this->rbuffer = aligned_alloc(4096, this->buffer_count * 4096);
	this->wbuffer = aligned_alloc(4096, 4096 * this->buffer_count);

	// This is an input socket because the task **receives** data from another module and sends it to FPGA
	auto &p1 = this->create_task("send");
	auto p1s_X_N = this->template create_socket_in<D>(p1, "X_N", this->N);
	this->create_codelet(p1, [p1s_X_N](Module &m, Task &t) -> int
	{
		static_cast<FPGA<D>&>(m).send(static_cast<D*>(t[p1s_X_N].get_dataptr()));

		return status_t::SUCCESS;
	});

	// This is an output socket because the task receives data from the FPGA and **sends** to another module
	auto &p2 = this->create_task("receive");
	auto p2s_Y_N = this->template create_socket_out<D>(p2, "Y_N", this->N);
	this->create_codelet(p2, [p2s_Y_N](Module &m, Task &t) -> int
	{
		static_cast<FPGA<D>&>(m).receive(static_cast<D*>(t[p2s_Y_N].get_dataptr()));

		return status_t::SUCCESS;
	});
}

template <typename D>
FPGA<D>
::~FPGA()
{
	// Closing opened device files
	fclose(this->fs_read);
	fclose(this->fs_write);

	// Deallocate memory
	std::free(this->wbuffer);
	std::free(this->rbuffer);
}

template <typename D>
FPGA<D>* FPGA<D>
::clone() const
{
	auto m = new FPGA(*this);
	m->deep_copy(*this);
	return m;
}

template <typename D>
int FPGA<D>
::get_N() const
{
	return N;
}

template <typename D>
template <class A>
void FPGA<D>
::send(std::vector<D,A>& X_N, const int frame_id)
{
	if (this->N * this->n_frames != (int)X_N.size())
	{
		std::stringstream message;
		message << "'X_N.size()' has to be equal to 'N' * 'n_frames' ('X_N.size()' = " << X_N.size()
		        << ", 'N' = " << this->N << ", 'n_frames' = " << this->n_frames << ").";
		throw tools::length_error(__FILE__, __LINE__, __func__, message.str());
	}

	this->send(X_N.data, frame_id);
}

template <typename D>
void FPGA<D>
::send(D *X_N, const int frame_id)
{
	const auto f_start = (frame_id < 0) ? 0 : frame_id % this->n_frames;
	const auto f_stop  = (frame_id < 0) ? this->n_frames : f_start +1;

	for (auto f = f_start; f < f_stop; f++)
		this->_send(X_N + f * this->N, f);
}

template <typename D>
void FPGA<D>
::_send(D *X_N, const int frame_id)
{
	/*
	wbuffer is reserved in the module constructor. It's an aligned memory block which size is a multiple of 4096.
	A multiple of 4096 is mandatory for the XDMA memory descriptors to work properly.
	*/
	std::memcpy(this->wbuffer, X_N, sizeof(D) * this->N * this->n_frames);

	// Writing to the FPGA memory. This function will block process until XDMA transfer is over.
	aff3ct::tools::write_to_device(this->fs_write, this->wbuffer, 4096, this->buffer_count, 0xc0000000);
	
	/*
	// Monitoring what bits we wrote.
	for (int i = 0; i < this->N; i++){
		printf("Wrote : %d \n", *((int*)this->wbuffer + i));
	}
	*/

	// We now poll on event device to handle an interrupt from card, telling FPGA process is done !
	int fd0 = open("/dev/xdma0_events_0", O_RDONLY);
    struct pollfd fds[] = {
        {fd0, POLLIN}
    };

    int r = poll(fds, 1, 1000); // 1sec timeout
    int read_fd;

    if (r < 0){
		std::stringstream message;
		message << "Error polling event";
        throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
    } else if (r == 0) {
		std::stringstream message;
		message << "Timeout error : no event received";
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	} else {
        if(fds[0].revents & POLLIN) {
            std::cout << "Event triggered !" << std::endl;
            read_fd = fd0;
        }
        uint32_t events_user;
        pread(read_fd, &events_user, sizeof(events_user), 0);
    }
    close(fd0);

}

template <typename D>
template <class A>
void FPGA<D>
::receive(std::vector<D,A>& Y_N, const int frame_id)
{
	if (this->N * this->n_frames != (int)Y_N.size())
	{
		std::stringstream message;
		message << "'Y_N.size()' has to be equal to 'N' * 'n_frames' ('Y_N.size()' = " << Y_N.size()
		        << ", 'N' = " << this->N << ", 'n_frames' = " << this->n_frames << ").";
		throw tools::length_error(__FILE__, __LINE__, __func__, message.str());
	}

	this->receive(Y_N.data(), frame_id);
}

template <typename D>
void FPGA<D>
::receive(D *Y_N, const int frame_id)
{
	const auto f_start = (frame_id < 0) ? 0 : frame_id % this->n_frames;
	const auto f_stop  = (frame_id < 0) ? this->n_frames : f_start +1;

	for (auto f = f_start; f < f_stop; f++)
		this->_receive(Y_N + f * this->N, f);
}

template <typename D>
void FPGA<D>
::_receive(D *Y_N, const int frame_id)
{	
	// Reading to the FPGA memory. This function will block process until XDMA transfer is over.
	aff3ct::tools::read_from_device(this->fs_read, this->rbuffer, 4096, this->buffer_count, 0xc0000000);

	/*
	rbuffer is reserved in the module constructor. It's an aligned memory block which size is a multiple of 4096.
	A multiple of 4096 is mandatory for the XDMA memory descriptors to work properly.
	*/
	std::memcpy(Y_N, this->rbuffer, sizeof(D) * this->N * this->n_frames);
	
	/*
	// Monitoring what we read from the FPGA.
	for (int i = 0; i < this->N; i++){
		printf("Read : %d \n", *((int*)this->rbuffer + i));
	}
	*/
}


}

}