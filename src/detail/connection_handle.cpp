/**
 * Copyright © 2014-2015, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "connection_handle.h"

#include <sqlpp11/postgresql/connection_config.h>
#include <sqlpp11/exception.h>

#include <iostream> // DEBUG

namespace sqlpp {

	namespace postgresql {

		namespace detail {

			connection_handle::connection_handle(const std::shared_ptr<connection_config> &conf) : config(conf) {

				if (config->debug) {
					std::cerr << "PostgreSQL debug: connecting to the database server." << std::endl;
				}

				// Open connection
				std::string conninfo = "";
				if (!config->host.empty()) {
					conninfo.append("host=" + config->host);
				}
				if (!config->hostaddr.empty()) {
					conninfo.append(" hostaddr=" + config->hostaddr);
				}
				if (config->port != 5432) {
					conninfo.append(" port=" + std::to_string(config->port));
				}
				if (!config->dbname.empty()) {
					conninfo.append(" dbname=" + config->dbname);
				}
				if (!config->user.empty()) {
					conninfo.append(" user=" + config->user);
				}
				if (!config->password.empty()) {
					conninfo.append(" password=" + config->password);
				}
				if (config->connect_timeout != 0) {
					conninfo.append(" connect_timeout=" + std::to_string(config->connect_timeout));
				}
				if (!config->client_encoding.empty()) {
					conninfo.append(" client_encoding=" + config->client_encoding);
				}
				if (!config->options.empty()) {
					conninfo.append(" options=" + config->options);
				}
				if (!config->application_name.empty()) {
					conninfo.append(" application_name=" + config->application_name);
				}
				if (!config->fallback_application_name.empty()) {
					conninfo.append(" fallback_application_name=" + config->fallback_application_name);
				}
				if (!config->keepalives) {
					conninfo.append(" keepalives=0");
				}
				if (config->keepalives_idle != 0) {
					conninfo.append(" keepalives_idle=" + std::to_string(config->keepalives_idle));
				}
				if (config->keepalives_interval != 0) {
					conninfo.append(" keepalives_interval=" + std::to_string(config->keepalives_interval));
				}
				if (config->keepalives_count != 0) {
					conninfo.append(" keepalives_count=" + std::to_string(config->keepalives_count));
				}
				switch(config->sslmode) {
					case connection_config::sslmode_t::disable:
						conninfo.append(" sslmode=disable");
						break;
					case connection_config::sslmode_t::allow:
						conninfo.append(" sslmode=allow");
						break;
					case connection_config::sslmode_t::require:
						conninfo.append(" sslmode=require");
						break;
					case connection_config::sslmode_t::verify_ca:
						conninfo.append(" sslmode=verify-ca");
						break;
					case connection_config::sslmode_t::verify_full:
						conninfo.append(" sslmode=verify-full");
						break;
					case connection_config::sslmode_t::prefer:
					default:
						break;
				}
				if (!config->sslcompression) {
					conninfo.append(" sslcompression=0");
				}
				if (!config->sslcert.empty()) {
					conninfo.append(" sslcert=" + config->sslcert);
				}
				if (!config->sslkey.empty()) {
					conninfo.append(" sslkey=" + config->sslkey);
				}
				if (!config->sslrootcert.empty()) {
					conninfo.append(" sslrootcert=" + config->sslrootcert);
				}
				if (!config->requirepeer.empty()) {
					conninfo.append(" requirepeer=" + config->requirepeer);
				}
				if (!config->krbsrvname.empty()) {
					conninfo.append(" krbsrvname=" + config->krbsrvname);
				}
				if (!config->service.empty()) {
					conninfo.append(" service=" + config->service);
				}
				this->postgres = PQconnectdb(conninfo.c_str());
				if (PQstatus(this->postgres) != CONNECTION_OK) {
					throw sqlpp::exception("PostgreSQL error: failed to connect to the database");
				}
			}

			connection_handle::~connection_handle() {

				// Debug
				if (config->debug) {
					std::cerr << "PostgreSQL debug: closing database connection." << std::endl;
				}

				// Close connection
				if (this->postgres) {
					PQfinish(this->postgres);
				}
			}
		}
	}
}

