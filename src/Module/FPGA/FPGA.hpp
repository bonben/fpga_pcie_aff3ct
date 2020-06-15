/*!
 * \file
 * \brief Class module::FPGA.
 */
#ifndef FPGA_HPP_
#define FPGA_HPP_

#include <cstdint>
#include <memory>
#include <vector>

#include "Module/Task.hpp"
#include "Module/Socket.hpp"
#include "Module/Module.hpp"

namespace aff3ct
{
namespace module
{
	namespace fpg
	{
		enum class tsk : size_t { send, receive, SIZE };

		namespace sck
		{
			enum class send    : size_t { X_N, status };
			enum class receive : size_t { Y_N, status };
		}
	}

/*!
 * \class FPGA
 *
 * \brief Exchanges data with FPGA through PCIe.
 *
 * \tparam D: type of the data to be exchanged.
 *
 */
template <typename D = int>
class FPGA : public Module
{
public:
	inline Task&   operator[](const fpg::tsk          t);
	inline Socket& operator[](const fpg::sck::send    s);
    inline Socket& operator[](const fpg::sck::receive s);

protected:
	const int N; /*!< Size of one frame */

	std::vector<D> data;

public:
	/*!
	 * \brief Constructor.
	 *
	 * \param N:        frame size.
	 * \param n_frames: number of frames to process.
	 */
	FPGA(const int N, const int n_frames = 1);

	/*!
	 * \brief Destructor.
	 */
	virtual ~FPGA() = default;

	virtual FPGA<D>* clone() const;

	virtual int get_N() const;

	/*!
	 * \brief Sends data to FPGA
	 *
	 * \param X_N: data to be sent.
	 */
	template <class A = std::allocator<D>>
	void send(std::vector<D,A>& X_N, const int frame_id = -1);

	virtual void send(D *X_N, const int frame_id = -1);

	/*!
	 * \brief Sends data to FPGA
	 *
	 * \param X_N: data to be sent.
	 */
	template <class A = std::allocator<D>>
	void receive(std::vector<D,A>& Y_N, const int frame_id = -1);

	virtual void receive(D *Y_N, const int frame_id = -1);

protected:
	virtual void _send   (D *X_N, const int frame_id);
	virtual void _receive(D *Y_N, const int frame_id);
};
}
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "Module/FPGA/FPGA.hxx"
#endif

#endif /* FPGA_HPP_ */
