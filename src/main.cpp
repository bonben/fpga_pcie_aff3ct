#include <iostream>
#include <aff3ct.hpp>

#include "Module/FPGA/FPGA.hpp"
#include "Factory/Module/FPGA/FPGA.hpp"
using namespace aff3ct;

struct params
{
	std::unique_ptr<factory::Source      > source;
	std::unique_ptr<factory::FPGA        > fpga;
	std::unique_ptr<factory::Monitor_BFER> monitor;
	std::unique_ptr<factory::Terminal    > terminal;
};
void init_params(int argc, char** argv, params &p);

struct modules
{
	std::unique_ptr<module::Source<>>       source;
	std::unique_ptr<module::FPGA<>>         fpga;
	std::unique_ptr<module::Monitor_BFER<>> monitor;
	std::vector<const module::Module*>      list; // list of module pointers declared in this structure
};
void init_modules(const params &p, modules &m);

struct utils
{
	std::vector<std::unique_ptr<tools::Reporter>> reporters; // list of reporters dispayed in the terminal
	            std::unique_ptr<tools::Terminal>  terminal;  // manage the output text in the terminal
};
void init_utils(const params &p, const modules &m, utils &u);

int main(int argc, char** argv)
{
	params  p; init_params (argc, argv, p  ); // create and initialize the parameters from the command line with factories
	modules m; init_modules(p, m           ); // create and initialize the modules
	utils   u; init_utils  (p, m, u        ); // create and initialize the utils

	// display the legend in the terminal
	u.terminal->legend();

	// sockets binding (connect the sockets of the tasks = fill the input sockets with the output sockets)
	using namespace module;
	(*m.fpga   )[fpg::sck::send        ::X_N ].bind((*m.source )[src::sck::generate   ::U_K]);
	(*m.monitor)[mnt::sck::check_errors::U   ].bind((*m.source )[src::sck::generate   ::U_K]);
	(*m.monitor)[mnt::sck::check_errors::V   ].bind((*m.fpga   )[fpg::sck::receive    ::Y_N]);

	// display the performance (BER and FER) in real time (in a separate thread)
	u.terminal->start_temp_report();

	// run the simulation chain for a certain number of frames
	// while (!u.terminal->is_interrupt())
	for (auto i = 0; i < 10; i++)
	{
		(*m.source )[src::tsk::generate    ].exec();
		(*m.fpga   )[fpg::tsk::send        ].exec();
		(*m.fpga   )[fpg::tsk::receive     ].exec();
		(*m.monitor)[mnt::tsk::check_errors].exec();
	}

	// display the performance (BER and FER) in the terminal
	u.terminal->final_report();
	// reset the monitor and the terminal for the next SNR

	/*
	m.monitor->reset();
	u.terminal->reset();
	*/

	// display the statistics of the tasks (if enabled)
	std::cout << "#" << std::endl;
	tools::Stats::show(m.list, true);
	std::cout << "# End of the simulation" << std::endl;

	return 0;
}


void init_params(int argc, char** argv, params &p)
{
	p.source   = std::unique_ptr<factory::Source          >(new factory::Source          ());
	p.fpga     = std::unique_ptr<factory::FPGA            >(new factory::FPGA            ());
	p.monitor  = std::unique_ptr<factory::Monitor_BFER    >(new factory::Monitor_BFER    ());
	p.terminal = std::unique_ptr<factory::Terminal        >(new factory::Terminal        ());

	std::vector<factory::Factory*> params_list = { p.source.get(), p.monitor.get(), p.fpga.get(), p.terminal.get() };

	// parse the command for the given parameters and fill them
	tools::Command_parser cp(argc, argv, params_list, true);
	if (cp.parsing_failed())
	{
		cp.print_help    ();
		cp.print_warnings();
		cp.print_errors  ();
		std::exit(1);
	}

	std::cout << "# Simulation parameters: " << std::endl;
	tools::Header::print_parameters(params_list); // display the headers (= print the AFF3CT parameters on the screen)
	std::cout << "#" << std::endl;
	cp.print_warnings();
}

void init_modules(const params &p, modules &m)
{	
	m.source  = std::unique_ptr<module::Source      <>>(p.source ->build<>());
	m.fpga    = std::unique_ptr<module::FPGA        <>>(p.fpga   ->build<>());
	m.monitor = std::unique_ptr<module::Monitor_BFER<>>(p.monitor->build<>());

	m.list = { m.source.get(), m.monitor.get(), m.fpga.get() };

	// configuration of the module tasks
	for (auto& mod : m.list)
		for (auto& tsk : mod->tasks)
		{
			tsk->set_autoalloc  (true ); // enable the automatic allocation of the data in the tasks
			tsk->set_debug      (false); // disable the debug mode
			tsk->set_debug_limit(16   ); // display only the 16 first bits if the debug mode is enabled
			tsk->set_stats      (true ); // enable the statistics

			// enable the fast mode (= disable the useless verifs in the tasks) if there is no debug and stats modes
			if (!tsk->is_debug() && !tsk->is_stats())
				tsk->set_fast(true);
		}
}

void init_utils(const params &p, const modules &m, utils &u)
{
	// report the bit/frame error rates
	u.reporters.push_back(std::unique_ptr<tools::Reporter>(new tools::Reporter_BFER<>(*m.monitor)));
	// report the simulation throughputs
	u.reporters.push_back(std::unique_ptr<tools::Reporter>(new tools::Reporter_throughput<>(*m.monitor)));
	// create a terminal that will display the collected data from the reporters
	u.terminal = std::unique_ptr<tools::Terminal>(p.terminal->build(u.reporters));
}