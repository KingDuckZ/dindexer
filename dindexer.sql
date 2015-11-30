--
-- PostgreSQL database dump
--

-- Dumped from database version 9.4.5
-- Dumped by pg_dump version 9.4.5
-- Started on 2015-11-30 18:28:52 GMT

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 178 (class 3079 OID 11869)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2038 (class 0 OID 0)
-- Dependencies: 178
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 191 (class 1255 OID 31290)
-- Name: upcase_hash(); Type: FUNCTION; Schema: public; Owner: @USERNAME@
--

CREATE FUNCTION upcase_hash() RETURNS trigger
    LANGUAGE plpgsql IMMUTABLE
    AS $$
    BEGIN        
        NEW."hash" = UPPER(NEW."hash");
        RETURN NEW;
    END;
$$;


ALTER FUNCTION public.upcase_hash() OWNER TO @USERNAME@;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 175 (class 1259 OID 31281)
-- Name: files; Type: TABLE; Schema: public; Owner: @USERNAME@; Tablespace: 
--

CREATE TABLE files (
    id bigint NOT NULL,
    path text NOT NULL,
    level smallint NOT NULL,
    group_id integer NOT NULL,
    is_directory boolean NOT NULL,
    is_symlink boolean NOT NULL,
    size bigint,
    hash character(48) NOT NULL,
    is_hash_valid boolean DEFAULT true NOT NULL,
    CONSTRAINT chk_files_dirsize_zero CHECK (((is_directory = false) OR (size = 0)))
);


ALTER TABLE files OWNER TO @USERNAME@;

--
-- TOC entry 174 (class 1259 OID 31279)
-- Name: files_id_seq; Type: SEQUENCE; Schema: public; Owner: @USERNAME@
--

CREATE SEQUENCE files_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE files_id_seq OWNER TO @USERNAME@;

--
-- TOC entry 2039 (class 0 OID 0)
-- Dependencies: 174
-- Name: files_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: @USERNAME@
--

ALTER SEQUENCE files_id_seq OWNED BY files.id;


--
-- TOC entry 177 (class 1259 OID 31411)
-- Name: sets; Type: TABLE; Schema: public; Owner: @USERNAME@; Tablespace: 
--

CREATE TABLE sets (
    id integer NOT NULL,
    "desc" text NOT NULL,
    type character(1) DEFAULT 'D'::bpchar NOT NULL,
    disk_number integer DEFAULT 0 NOT NULL,
    creation timestamp with time zone DEFAULT now() NOT NULL,
    CONSTRAINT chk_sets_type CHECK ((((((((type = 'D'::bpchar) OR (type = 'V'::bpchar)) OR (type = 'B'::bpchar)) OR (type = 'F'::bpchar)) OR (type = 'H'::bpchar)) OR (type = 'Z'::bpchar)) OR (type = 'O'::bpchar)))
);


ALTER TABLE sets OWNER TO @USERNAME@;

--
-- TOC entry 2040 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN sets.type; Type: COMMENT; Schema: public; Owner: @USERNAME@
--

COMMENT ON COLUMN sets.type IS 'D = directory
V = DVD
B = BluRay
F = Floppy Disk
H = Hard Disk
Z = Iomega Zip
O = Other';


--
-- TOC entry 176 (class 1259 OID 31409)
-- Name: sets_id_seq; Type: SEQUENCE; Schema: public; Owner: @USERNAME@
--

CREATE SEQUENCE sets_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE sets_id_seq OWNER TO @USERNAME@;

--
-- TOC entry 2041 (class 0 OID 0)
-- Dependencies: 176
-- Name: sets_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: @USERNAME@
--

ALTER SEQUENCE sets_id_seq OWNED BY sets.id;


--
-- TOC entry 1904 (class 2604 OID 31284)
-- Name: id; Type: DEFAULT; Schema: public; Owner: @USERNAME@
--

ALTER TABLE ONLY files ALTER COLUMN id SET DEFAULT nextval('files_id_seq'::regclass);


--
-- TOC entry 1907 (class 2604 OID 31414)
-- Name: id; Type: DEFAULT; Schema: public; Owner: @USERNAME@
--

ALTER TABLE ONLY sets ALTER COLUMN id SET DEFAULT nextval('sets_id_seq'::regclass);


--
-- TOC entry 1915 (class 2606 OID 31289)
-- Name: pk_files_id; Type: CONSTRAINT; Schema: public; Owner: @USERNAME@; Tablespace: 
--

ALTER TABLE ONLY files
    ADD CONSTRAINT pk_files_id PRIMARY KEY (id);


--
-- TOC entry 1919 (class 2606 OID 31420)
-- Name: pk_sets_id; Type: CONSTRAINT; Schema: public; Owner: @USERNAME@; Tablespace: 
--

ALTER TABLE ONLY sets
    ADD CONSTRAINT pk_sets_id PRIMARY KEY (id);


--
-- TOC entry 1917 (class 2606 OID 31294)
-- Name: uniq_item; Type: CONSTRAINT; Schema: public; Owner: @USERNAME@; Tablespace: 
--

ALTER TABLE ONLY files
    ADD CONSTRAINT uniq_item UNIQUE (path, group_id);


--
-- TOC entry 1912 (class 1259 OID 31426)
-- Name: fki_files_sets; Type: INDEX; Schema: public; Owner: @USERNAME@; Tablespace: 
--

CREATE INDEX fki_files_sets ON files USING btree (group_id);


--
-- TOC entry 1913 (class 1259 OID 31292)
-- Name: idx_paths; Type: INDEX; Schema: public; Owner: @USERNAME@; Tablespace: 
--

CREATE INDEX idx_paths ON files USING btree (path);


--
-- TOC entry 1921 (class 2620 OID 31291)
-- Name: triggerupcasehash; Type: TRIGGER; Schema: public; Owner: @USERNAME@
--

CREATE TRIGGER triggerupcasehash BEFORE INSERT OR UPDATE ON files FOR EACH ROW EXECUTE PROCEDURE upcase_hash();


--
-- TOC entry 1920 (class 2606 OID 31421)
-- Name: fk_files_sets; Type: FK CONSTRAINT; Schema: public; Owner: @USERNAME@
--

ALTER TABLE ONLY files
    ADD CONSTRAINT fk_files_sets FOREIGN KEY (group_id) REFERENCES sets(id);


--
-- TOC entry 2037 (class 0 OID 0)
-- Dependencies: 8
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2015-11-30 18:28:54 GMT

--
-- PostgreSQL database dump complete
--

