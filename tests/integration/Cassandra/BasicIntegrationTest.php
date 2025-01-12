<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace Cassandra;

use CCM;
use PHPUnit\Framework\TestCase;

/**
 * Basic/Simple integration test class to provide common integration test
 * functionality when a simple setup and teardown is required. This class
 * should be used for the majority of tests.
 */
abstract class BasicIntegrationTest extends TestCase
{
    /**
     * Conversion value for seconds to milliseconds.
     */
    public const SECONDS_TO_MILLISECONDS = 1000;
    /**
     * Conversion value for seconds to microseconds.
     */
    public const SECONDS_TO_MICROSECONDS = 1000000;

    /**
     * Integration test instance (helper class).
     *
     * @var Integration
     */
    private $integration;

    /**
     * Number of nodes in data center one.
     *
     * @var int
     */
    protected $numberDC1Nodes = 1;
    /**
     * Number of nodes in data center two.
     *
     * @var int
     */
    protected $numberDC2Nodes = 0;
    /**
     * Replication factor override.
     *
     * @var int
     */
    protected $replicationFactor = -1;

    /**
     * Established cluster configuration.
     *
     * @var Cluster
     */
    protected Cluster $cluster;
    /**
     * Connected database session.
     *
     * @var Session
     */
    protected Session $session;
    /**
     * Version of Cassandra/DSE the session is connected to.
     *
     * @var string
     */
    protected mixed $serverVersion;
    /**
     * Keyspace name being used for the test.
     *
     * @var string
     */
    protected $keyspaceName;
    /**
     * Table name prefix being used for the test.
     *
     * @var string
     */
    protected $tableNamePrefix;
    /**
     * Flag to determine if client authentication should be enabled.
     *
     * @var bool
     */
    protected $isClientAuthentication = false;
    /**
     * Flag to determine if SSL should be enabled.
     *
     * @var bool
     */
    protected $isSSL = false;
    /**
     * Flag to determine if UDA/UDF functionality should be enabled.
     *
     * @var bool
     */
    protected $isUserDefinedAggregatesFunctions = false;

    /**
     * Setup the database for the integration tests.
     */
    protected function setUp(): void
    {
        // Initialize the database and establish a connection
        $this->integration = new Integration(
            static::class,
            $this->getName(false),
            $this->numberDC1Nodes,
            $this->numberDC2Nodes,
            $this->replicationFactor,
            $this->isClientAuthentication,
            $this->isSSL,
            $this->isUserDefinedAggregatesFunctions
        );
        $this->ccm = $this->integration->ccm;
        $this->cluster = $this->integration->cluster;
        $this->session = $this->integration->session;
        $this->serverVersion = $this->integration->serverVersion;

        // Assign the keyspace and table name for the test
        $this->keyspaceName = strtolower($this->integration->keyspaceName);
        $this->tableNamePrefix = strtolower($this->getName(false));
    }

    /**
     * Teardown the database for the integration tests.
     */
    protected function tearDown(): void
    {
        unset($this->integration);
        unset($this->ccm);
        unset($this->session);
    }
}
