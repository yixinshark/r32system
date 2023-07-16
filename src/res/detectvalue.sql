<?xml version="1.0" encoding="UTF-8"?>
<sqls namespace="R32Table">
    <define id="fields">id, serial_number, dateTime, sensor_id, sensor_addr, channel, point, cal_concentration, temperature, humidity, gas_adc, get_concentration</define>

    <sql id="CreatTable">
        CREATE TABLE r32table (
            id INT IDENTITY(1,1) PRIMARY KEY,
            serial_number VARCHAR(50),
            dateTime VARCHAR(50),
            sensor_id VARCHAR(50) UNIQUE,
            sensor_addr VARCHAR(50),
            channel VARCHAR(50),
            point INT,
            cal_concentration FLOAT,
            temperature FLOAT,
            humidity FLOAT,
            gas_adc FLOAT,
            get_concentration FLOAT
        );
    </sql>

    <sql id="CheckTable">
        SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'r32table'
    </sql>

    <sql id="insert">
        INSERT INTO r32table ( serial_number, dateTime, sensor_id, sensor_addr, channel, point, cal_concentration, temperature, humidity, gas_adc, get_concentration)
        VALUES (:serial_number, :dateTime, :sensor_id, :sensor_addr, :channel, :point, :cal_concentration, :temperature, :humidity, :gas_adc, :get_concentration)
    </sql>

    <sql id="update">
        UPDATE r32table SET adc_value = :adc_value, concentration = :concentration WHERE sensor_id = :sensor_id and point = :point
    </sql>

    <sql id="countBySensorIdAndPoint">
        SELECT COUNT(*) FROM r32table WHERE sensor_id = '%1' and point = %2
    </sql>

    <sql id="datasCount">
        SELECT count(*) FROM r32table WHERE dateTime &gt;= '%1' and dateTime &lt;= '%2'
    </sql>

    <sql id="findByDateTime">
        SELECT <include defineId="fields"/> FROM r32table WHERE dateTime &gt;= '%1' and dateTime &lt;= '%2' ORDER BY dateTime
    </sql>

    <sql id="deleteByDateTime">
        DELETE FROM r32table WHERE dateTime &lt;= '%1'
    </sql>
</sqls>