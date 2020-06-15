#include <string>
#include <sstream>

#include "Tools/Exception/exception.hpp"
#include "Module/FPGA/FPGA.hpp"

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
: Module(n_frames), N(N), data(N)
{
	const std::string name = "FPGA";
	this->set_name(name);
	this->set_short_name(name);

	if (N <= 0)
	{
		std::stringstream message;
		message << "'N' has to be greater than 0 ('N' = " << N << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	auto &p1 = this->create_task("send");
	auto p1s_X_N = this->template create_socket_out<D>(p1, "X_N", this->N);
	this->create_codelet(p1, [p1s_X_N](Module &m, Task &t) -> int
	{
		static_cast<FPGA<D>&>(m).send(static_cast<D*>(t[p1s_X_N].get_dataptr()));

		return status_t::SUCCESS;
	});

	auto &p2 = this->create_task("receive");
	auto p2s_Y_N = this->template create_socket_out<D>(p2, "Y_N", this->N);
	this->create_codelet(p2, [p2s_Y_N](Module &m, Task &t) -> int
	{
		static_cast<FPGA<D>&>(m).receive(static_cast<D*>(t[p2s_Y_N].get_dataptr()));

		return status_t::SUCCESS;
	});
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

	this->send(X_N.data(), frame_id);
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
	std::copy(X_N, X_N + this->N, data.data());
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
	std::copy(data.data(), data.data() + this->N, Y_N);
}

}
}
