-- dofile("C:/Users/ricca/Desktop/disqube/lua/discover_hello.lua")

-- Protocol: DISCOVER HELLO
-- Sent by the Qube Master to the Qube Workers.
-- It is a UDP message with 12 byte of payload divided into
-- 8 bytes of Common Header and 4 bytes of actual data

Discover_hello = Proto("Discover_hello", "DISCOVER HELLO Protocol")

-- Defining the fields for the protocol
local f_tcp_prt = ProtoField.uint16("Discover_hello.tcp_prt", "TCP PORT", base.DEC)
local f_udp_prt = ProtoField.uint16("Discover_hello.udp_prt", "UDP PORT", base.DEC)
local f_src_addr = ProtoField.uint32("Discover_hello.src_addr", "SRC IP ADDR", base.HEX)

Discover_hello.fields = { F_id, F_counter, F_flag, F_subtype, F_type, f_tcp_prt, f_udp_prt, f_src_addr }

-- Dissector Functior
function Discover_hello.dissector(buffer, pinfo, tree)
    -- Check the buffer has enough length
    if buffer:len() < 16 and buffer:len() > 16 then
        return
    end

    -- Check that the subtype is DISCOVER HELLO
    if CommonHeader.getHeaderSubtype(buffer) ~= 1 then
        return
    end

    pinfo.cols.protocol = "DISCOVER HELLO"

    local subtree = tree:add(Discover_hello, buffer(), "Discover Hello Data")

    -- Dissect the Common header
    local remain_len = CommonHeader.dissect_common_header(buffer, subtree)

    -- Get the Discover hello data
    local udp_prt = buffer(remain_len, 2):le_uint()
    local tcp_prt = buffer(remain_len + 2, 2):le_uint()
    local src_addr = buffer(remain_len + 4, 4):le_uint()

    subtree:add(f_udp_prt, udp_prt) -- MESSAGE DATA: UDP PORT
    subtree:add(f_tcp_prt, tcp_prt) -- MESSAGE DATA: TCP PORT
    subtree:add(f_src_addr, src_addr) -- MESSAGE DATA: SRC IP ADDR
end

local udp = DissectorTable.get("udp.port")
udp:add(32125, Discover_hello)
udp:add(33333, Discover_hello)