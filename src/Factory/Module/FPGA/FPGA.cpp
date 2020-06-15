#include <utility>

#include "Module/FPGA/FPGA.hpp"
#include "Factory/Module/FPGA/FPGA.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::FPGA_name   = "FPGA";
const std::string aff3ct::factory::FPGA_prefix = "fpg";

FPGA
::FPGA(const std::string &prefix)
: Factory(FPGA_name, FPGA_name, prefix)
{
}

FPGA
::FPGA(const std::string &name, const std::string &prefix)
: Factory(name, FPGA_name, prefix)
{
}

FPGA* FPGA
::clone() const
{
	return new FPGA(*this);
}

void FPGA
::get_description(cli::Argument_map_info &args) const
{
	auto p = this->get_prefix();
	const std::string class_name = "factory::FPGA::";

	args.add({p+"-fra-size"  ,"N" }, cli::Integer(cli::Positive(), cli::Non_zero()), "Frame size");
	args.add({p+"-fra"       ,"F" }, cli::Integer(cli::Positive(), cli::Non_zero()), "Inter frame lvel");
}

void FPGA
::store(const cli::Argument_map_value &vals)
{
	auto p = this->get_prefix();

	if(vals.exist({p+"-fra-size",  "N"})) this->N        = vals.to_int({p+"-fra-size",  "N"});
	if(vals.exist({p+"-fra",       "F"})) this->n_frames = vals.to_int({p+"-fra",       "F"});
}

void FPGA
::get_headers(std::map<std::string,tools::header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Frame size (N)",    std::to_string(this->N)));
	headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
}

template <typename D>
module::FPGA<D>* FPGA
::build() const
{
	return new module::FPGA<D>(this->N, this->n_frames);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
template aff3ct::module::FPGA<int8_t >* aff3ct::factory::FPGA::build<int8_t >() const;
template aff3ct::module::FPGA<int16_t>* aff3ct::factory::FPGA::build<int16_t>() const;
template aff3ct::module::FPGA<int32_t>* aff3ct::factory::FPGA::build<int32_t>() const;
template aff3ct::module::FPGA<int64_t>* aff3ct::factory::FPGA::build<int64_t>() const;
template aff3ct::module::FPGA<float  >* aff3ct::factory::FPGA::build<float  >() const;
template aff3ct::module::FPGA<double >* aff3ct::factory::FPGA::build<double >() const;
// ==================================================================================== explicit template instantiation
