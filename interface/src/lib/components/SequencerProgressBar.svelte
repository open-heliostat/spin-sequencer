<script lang="ts">
	import type { SequencerState, SequencerStatus } from "$lib/types/models";
	import Slider from "./Slider.svelte";

    export let sequencerStatus: SequencerStatus;

</script>

<Slider
    label="Command"
    min={0}
    max={sequencerStatus.numCommands-1}
    step={1}
    disabled
    bind:value={sequencerStatus.selectedCommand}
/>
<Slider
    label="Progress"
    min={0}
    max={sequencerStatus.commandDuration}
    step={1}
    disabled
    value={sequencerStatus.commandDuration - sequencerStatus.commandDurationLeft} 
/>
