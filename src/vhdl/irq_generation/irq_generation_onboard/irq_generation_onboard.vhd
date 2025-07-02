library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity irq_generation_onboard is
  port
  (
    clk      : in  std_logic;
    rst      : in  std_logic;
    buttons  : in  std_logic_vector(3 downto 0);
    switches : in  std_logic_vector(3 downto 0);
    leds     : out std_logic_vector(3 downto 0)
  );
end entity;

architecture rtl of irq_generation_onboard is

  signal irq_source_in       : std_logic_vector(3 downto 0);
  signal irq_clear_in        : std_logic_vector(3 downto 0);
  signal irq_enable_in       : std_logic_vector(3 downto 0);
  signal irq_pending_out     : std_logic_vector(3 downto 0);
  signal irq_out             : std_logic;

  type fsm_state_type is (IDLE, CLEAR);
  signal fsm_state           : fsm_state_type;
  signal clear_counter       : integer range 0 to 15;

  signal button_sync_0_q     : std_logic;
  signal button_sync_1_q     : std_logic;
  signal switch_sync_0_q     : std_logic;
  signal switch_sync_1_q     : std_logic;

  -- constant DEBOUNCE_LIMIT    : integer := 50_000;  -- adjust for ~1ms at 50 MHz
  constant DEBOUNCE_LIMIT    : integer := 5;  -- adjust for ~1ms at 50 MHz
  signal debounce_cnt        : integer range 0 to 1_000_000;
  signal button_debounced_q  : std_logic;

begin

  sync_inputs_proc : process (clk)
  begin
    if (rising_edge(clk)) then
      if (rst = '1') then
        button_sync_0_q <= '0';
        button_sync_1_q <= '0';
        switch_sync_0_q <= '0';
        switch_sync_1_q <= '0';
      else
        button_sync_0_q <= buttons(0);
        button_sync_1_q <= button_sync_0_q;
        switch_sync_0_q <= switches(0);
        switch_sync_1_q <= switch_sync_0_q;
      end if;
    end if;
  end process;

  debounce_btn_proc : process (clk)
  begin
    if (rising_edge(clk)) then
      if (rst = '1') then
        debounce_cnt         <= 0;
        button_debounced_q   <= '0';
      else if (button_debounced_q = button_sync_1_q) then
        debounce_cnt         <= 0;
      else
        debounce_cnt         <= debounce_cnt + 1;
        if (debounce_cnt > DEBOUNCE_LIMIT) then
          button_debounced_q <= button_sync_1_q;
        end if;
      end if;
    end if;
  end if;
  end process;

  -- FSM to pulse clear signal
  clear_fsm : process(clk)
  begin
    if rising_edge(clk) then
      if rst = '1' then
        clear_counter <= 0;
        irq_clear_in  <= (others => '0');
        fsm_state     <= IDLE;
      else
        case fsm_state is
          when IDLE =>
            if irq_out = '1' then
              clear_counter <= 0;
              irq_clear_in  <= (others => '1');
              fsm_state     <= CLEAR;
            else
              irq_clear_in  <= (others => '0');
            end if;

          when CLEAR =>
            if clear_counter < 2 then
              clear_counter <= clear_counter + 1;
              irq_clear_in  <= (others => '1');
              -- FSM stays in CLEAR
            else
              irq_clear_in <= (others => '0');
              fsm_state    <= IDLE;
            end if;
        end case;
      end if;
    end if;
  end process;

  irq_source_in(0) <= button_debounced_q;
  irq_enable_in(0) <= switch_sync_1_q;

  u_irq_gen: entity work.irq_generation
    generic map
    (
      GPIO_WIDTH => 4
    )
    port map
    (
      clock           => clk,
      reset           => rst,
      irq_source_in   => irq_source_in,
      irq_enable_in   => irq_enable_in,
      irq_clear_in    => irq_clear_in,
      irq_pending_out => irq_pending_out,
      irq_out         => irq_out
    );

  leds(0) <= irq_pending_out(0);
  leds(3) <= irq_out;

end architecture;
