<?xml version="1.0" encoding="UTF-8"?>
<sqls namespace="R32Table">
    <define id="fields">id,dateTime,sensor_id,valid,on_off,soft_version,point1,point2,point3,cal_status,R0,R1000,R5000,p,p1,p2,temperature,humidity,ccr5000,r32Ccr5000,ccr3000,r32Ccr3000,ccr1000,r32Ccr1000,ccr500,r32Ccr500,ccr0,r32Ccr0</define>

    <sql id="CreatTable">
        CREATE TABLE r32table (
            id INT IDENTITY(1,1) PRIMARY KEY,
            dateTime VARCHAR(50),
            sensor_id VARCHAR(50),
            valid VARCHAR(50),
            on_off VARCHAR(50),
            soft_version VARCHAR(50),
            point1 INT,
            point2 INT,
            point3 INT,
            cal_status bit,
            R0 FLOAT,
            R1000 FLOAT,
            R5000 FLOAT,
            p FLOAT,
            p1 FLOAT,
            p2 FLOAT,
            temperature FLOAT,
            humidity FLOAT,
            ccr5000 INT,
            r32Ccr5000 INT,
            ccr3000 INT,
            r32Ccr3000 INT,
            ccr1000 INT,
            r32Ccr1000 INT,
            ccr500 INT,
            r32Ccr500 INT,
            ccr0 INT,
            r32Ccr0 INT
        );
    </sql>

    <sql id="CheckTable">
        SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'r32table'
    </sql>

    <sql id="insert">
        INSERT INTO r32table ( dateTime,sensor_id,valid,on_off,soft_version,point1,point2,point3,cal_status,R0,R1000,R5000,p,p1,p2,temperature,humidity,ccr5000,r32Ccr5000,ccr3000,r32Ccr3000,ccr1000,r32Ccr1000,ccr500,r32Ccr500,ccr0,r32Ccr0)
        VALUES (:dateTime,:sensor_id,:valid,:on_off,:soft_version,:point1,:point2,:point3,:cal_status,:R0,:R1000,:R5000,:p,:p1,:p2,:temperature,:humidity,:ccr5000,:r32Ccr5000,:ccr3000,:r32Ccr3000,:ccr1000,:r32Ccr1000,:ccr500,:r32Ccr500,:ccr0,:r32Ccr0)
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