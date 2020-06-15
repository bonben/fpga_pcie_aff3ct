/*!
 * \file
 * \brief Class factory::FPGA.
 */
#ifndef FACTORY_FPGA_HPP_
#define FACTORY_FPGA_HPP_

#include <string>
#include <map>
#include <cli.hpp>

#include "Tools/Factory/Header.hpp"
#include "Module/FPGA/FPGA.hpp"
#include "Factory/Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string FPGA_name;
extern const std::string FPGA_prefix;
class FPGA : public Factory
{
public:
	// ----------------------------------------------------------------------------------------------------- PARAMETERS
	// required parameters
	int         N        = 0;

	// optional parameters
	int         n_frames = 1;

	// -------------------------------------------------------------------------------------------------------- METHODS
	explicit FPGA(const std::string &p = FPGA_prefix);
	virtual ~FPGA() = default;
	virtual FPGA* clone() const;

	// parameters construction
	virtual void get_description(cli::Argument_map_info &args) const;
	virtual void store          (const cli::Argument_map_value &vals);
	virtual void get_headers    (std::map<std::string,tools::header_list>& headers, const bool full = true) const;

	// builder
	template <typename D = int>
	module::FPGA<D>* build() const;

protected:
	FPGA(const std::string &n, const std::string &p);
};
}
}

#endif /* FACTORY_FPGA_HPP_ */
