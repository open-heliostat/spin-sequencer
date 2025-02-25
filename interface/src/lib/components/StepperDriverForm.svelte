<script lang="ts">
    import Collapsible from '$lib/components/Collapsible.svelte';
    import Settings from '~icons/tabler/adjustments';
    import { socket } from '$lib/stores/socket';
    import Slider from '$lib/components/Slider.svelte';
    import Checkbox from '$lib/components/Checkbox.svelte';
    import Text from '$lib/components/Text.svelte';
    import type { StepperDriver } from '$lib/types/models';

    export let driverSettings: StepperDriver;
    export let onChange: () => void;
</script>

<style>
    .grid-form {
        grid-template-columns: [labels] auto [controls] 1fr;
    }
</style>

<!-- <Collapsible open={false}>
    <Settings slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Driver Settings</span>
    <div class="w-full overflow-x-auto"> -->
        <form>
            <!-- Chopper Configuration -->
            <Collapsible>
                <span slot="title">Chopper Configuration</span>
                <div class="w-full grid grid-flow-row grid-form items-center">
                    <Slider
                        label="TOFF"
                        min={0}
                        max={15}
                        step={1}
                        bind:value={driverSettings.chopConfig.toff}
                        {onChange}
                    />
                    <Slider
                        label="HSTRT"
                        min={0}
                        max={7}
                        step={1}
                        bind:value={driverSettings.chopConfig.hstrt}
                        {onChange}
                    />
                    <Slider
                        label="HEND"
                        min={-3}
                        max={12}
                        step={1}
                        bind:value={driverSettings.chopConfig.hend}
                        {onChange}
                    />
                    <Slider
                        label="TBL"
                        min={0}
                        max={3}
                        step={1}
                        bind:value={driverSettings.chopConfig.tbl}
                        {onChange}
                    />
                    <Slider
                        label="Microsteps"
                        min={0}
                        max={8}
                        step={1}
                        bind:value={driverSettings.chopConfig.mres}
                        {onChange}
                    />
                </div>
            </Collapsible>

            <!-- Stallguard Configuration -->
            <Collapsible>
                <span slot="title">StallGuard Configuration</span>
                <div class="w-full grid grid-flow-row grid-form items-center">
                    <Checkbox
                        label="StallGuard Enable"
                        bind:value={driverSettings.stallConfig.sg_stop}
                        {onChange}
                    />
                    <Slider
                        label="SEMIN"
                        min={0}
                        max={15}
                        step={1}
                        bind:value={driverSettings.stallConfig.semin}
                        {onChange}
                    />
                    <Slider
                        label="SEMAX"
                        min={0}
                        max={15}
                        step={1}
                        bind:value={driverSettings.stallConfig.semax}
                        {onChange}
                    />
                    <Slider
                        label="SEDN"
                        min={0}
                        max={3}
                        step={1}
                        bind:value={driverSettings.stallConfig.sedn}
                        {onChange}
                    />
                    <Slider
                        label="SGT"
                        min={-64}
                        max={63}
                        step={1}
                        bind:value={driverSettings.stallConfig.sgt}
                        {onChange}
                    />
                </div>
            </Collapsible>

            <!-- PWM Configuration -->
            <Collapsible>
                <span slot="title">PWM Configuration</span>
                <div class="w-full grid grid-flow-row grid-form items-center">
                    <Slider
                        label="PWM Offset"
                        min={0}
                        max={255}
                        step={1}
                        bind:value={driverSettings.pwmConfig.pwm_ofs}
                        {onChange}
                    />
                    <Slider
                        label="PWM Gradient"
                        min={0}
                        max={255}
                        step={1}
                        bind:value={driverSettings.pwmConfig.pwm_grad}
                        {onChange}
                    />
                    <Slider
                        label="PWM Frequency"
                        min={0}
                        max={3}
                        step={1}
                        bind:value={driverSettings.pwmConfig.pwm_freq}
                        {onChange}
                    />
                    <Checkbox
                        label="PWM Autoscale"
                        bind:value={driverSettings.pwmConfig.pwm_autoscale}
                        {onChange}
                    />
                    <Checkbox
                        label="PWM Autograd"
                        bind:value={driverSettings.pwmConfig.pwm_autograd}
                        {onChange}
                    />
                </div>
            </Collapsible>

            <!-- Driver Status -->
            <div class="w-full mt-4">
                <h3 class="font-medium mb-2">Driver Status</h3>
                <div class="grid grid-cols-2 gap-2 text-sm">
                    <div>Enabled: {driverSettings.status?.enabled ? 'Yes' : 'No'}</div>
                    <div>StallGuard: {driverSettings.status?.stallguard ? 'Active' : 'Inactive'}</div>
                    <div>Overtemp: {driverSettings.status?.overtemp ? 'Yes' : 'No'}</div>
                    <div>Warning: {driverSettings.status?.overtemp_warning ? 'Yes' : 'No'}</div>
                    <div>CS Actual: {driverSettings.status?.cs_actual}</div>
                    <div>Current Scale: {driverSettings.status?.current_scale}</div>
                </div>
            </div>
        </form>
    <!-- </div>
</Collapsible> -->