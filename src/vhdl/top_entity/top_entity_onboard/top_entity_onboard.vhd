library ieee;
use ieee.std_logic_1164.all;

entity top_entity_onboard is
    port
    (
        clk      : in  std_logic;
        rst      : in  std_logic;
        buttons  : in  std_logic_vector(3 downto 0);
        switches : in  std_logic_vector(3 downto 0);
        leds     : out std_logic_vector(3 downto 0)
    );
end entity;

architecture data_flow of top_entity_onboard is

    component my_gpio is
        generic
        (
            GPIO_WIDTH  : natural := 4;
            INT_ENABLED : boolean := true
        );
        port
        (
            clock           : in    std_logic;
            reset           : in    std_logic;
            data_in         : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            data_out_en_in  : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_enable_in   : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_clear_in    : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_pending_out : out   std_logic_vector(GPIO_WIDTH-1 downto 0);
            irq_out         : out   std_logic;
            data_out        : out   std_logic_vector(GPIO_WIDTH-1 downto 0);
            gpio_pin_inout  : inout std_logic_vector(GPIO_WIDTH-1 downto 0)
        );
    end component;

    signal rst_sync_0    : std_logic;
    signal rst_sync_1    : std_logic;
    signal switch_sync_0 : std_logic;
    signal switch_sync_1 : std_logic;
    signal button_sync_0 : std_logic;
    signal button_sync_1 : std_logic;

    constant DEBOUNCE_THRESHOLD : integer := 5;
    signal   debounce_counter   : integer range 0 to 1_000_000;
    signal   debounced_button_q : std_logic;

    type   fsm_state_type is (IDLE, CLEAR);
    signal fsm_state     : fsm_state_type;
    signal clear_counter : integer range 0 to 10;

    signal data_in_sig         : std_logic_vector(3 downto 0);
    signal data_out_en_in_sig  : std_logic_vector(3 downto 0);
    signal int_enable_in_sig   : std_logic_vector(3 downto 0);
    signal int_clear_in_q      : std_logic_vector(3 downto 0);
    signal int_pending_out_sig : std_logic_vector(3 downto 0);
    signal irq_out_sig         : std_logic;
    signal data_out_sig        : std_logic_vector(3 downto 0);
    signal gpio_pin_inout_sig  : std_logic_vector(3 downto 0);
begin

    reset_sync_proc : process (clk)
    begin
        if (rising_edge(clk)) then
            rst_sync_0 <= rst;
            rst_sync_1 <= rst_sync_0;
        end if;
    end process;

    input_sync_proc : process (clk)
    begin
        if (rising_edge(clk)) then
            if (rst_sync_1 = '1') then
                switch_sync_0 <= '0';
                switch_sync_1 <= '0';
                button_sync_0 <= '0';
                button_sync_1 <= '0';
            else
                switch_sync_0 <= switches(0);
                switch_sync_1 <= switch_sync_0;
                button_sync_0 <= buttons(0);
                button_sync_1 <= button_sync_0;
            end if;
        end if;
    end process;

    button_debounce_proc : process (clk)
    begin
        if (rising_edge(clk)) then
            if (rst_sync_1 = '1') then
                debounce_counter   <= 0;
                debounced_button_q <= '0';
            else if (debounced_button_q = button_sync_1) then
                debounce_counter <= 0;
            else
                debounce_counter <= debounce_counter + 1;
                if (debounce_counter > DEBOUNCE_THRESHOLD) then
                    debounced_button_q <= button_sync_1;
                end if;
            end if;
        end if;
        end if;
    end process;

    fsm_pulse_clear : process (clk)
    begin
        if (rising_edge(clk)) then
            if (rst_sync_1 = '1') then
                clear_counter  <= 0;
                int_clear_in_q <= (others => '0');
                fsm_state      <= IDLE;
            else
                case fsm_state is
                    when IDLE =>
                        if (irq_out_sig = '1') then
                            clear_counter  <= 0;
                            int_clear_in_q <= (others => '1');
                            fsm_state <= CLEAR;
                        else
                            int_clear_in_q <= (others => '0');
                        end if;
                    when CLEAR =>
                        if (clear_counter < 2) then
                            clear_counter <= clear_counter + 1;
                            int_clear_in_q <= (others => '1');
                        else
                            int_clear_in_q <= (others => '1');
                            fsm_state <= CLEAR;
                        end if;
                end case;
            end if;
        end if;
    end process;

    data_in_sig(0)                 <= debounced_button_q;
    data_in_sig(3 downto 1)        <= (others => '0');
    data_out_en_in_sig(0)          <= switch_sync_1;
    data_out_en_in_sig(3 downto 1) <= (others => '0');
    int_enable_in_sig              <= "1111";

    my_gpio_inst : my_gpio
        generic map
        (
            GPIO_WIDTH  => 4,
            INT_ENABLED => true
        )
        port map
        (
            clock           => clk,
            reset           => rst_sync_1,
            data_in         => data_in_sig,
            data_out_en_in  => data_out_en_in_sig,
            int_enable_in   => int_enable_in_sig,
            int_clear_in    => int_clear_in_q,
            int_pending_out => int_pending_out_sig,
            irq_out         => irq_out_sig,
            data_out        => data_out_sig,
            gpio_pin_inout  => gpio_pin_inout_sig
        );

    leds(0)          <= gpio_pin_inout_sig(0);
    leds(1)          <= '0';
    leds(2)          <= int_pending_out_sig(0);
    leds(3)          <= irq_out_sig;

end architecture;