<script lang="ts">
	import Button from "$lib/components/Button.svelte";
	import GridForm from "$lib/components/GridForm.svelte";
	import SequencerProgressBar from "$lib/components/SequencerProgressBar.svelte";
	import SettingsCard from "$lib/components/SettingsCard.svelte";
	import { notifications } from "$lib/components/toasts/notifications";
	import WelcomeTextComp from "$lib/components/WelcomeTextComp.svelte";
	import WelcomeTextDisplayComp from "$lib/components/WelcomeTextDisplayComp.svelte";
	import { getJsonRest, postJsonRest } from "$lib/stores/rest";
	import type { SequencerStatus } from "$lib/types/models";
	import { onMount } from "svelte";

	let sequencerStatus: SequencerStatus;

    async function getSequencerStatus() {
        return getJsonRest("/rest/spin-seq/sequencer/status", sequencerStatus)
            .then((data) => {
                sequencerStatus = data;
            });
    }

	async function startSequencer() {
        return postJsonRest("/rest/spin-seq/sequencer", {control:{execute:1,run:true}});
	}

	async function stopSequencer() {
		return postJsonRest("/rest/spin-seq/sequencer", {control:{execute:0,run:false}});
	}

	onMount(() => {
		getSequencerStatus();
		const interval = setInterval(getSequencerStatus, 1000);
		return () => clearInterval(interval);
	});

</script>
<WelcomeTextDisplayComp/>

{#if sequencerStatus}
	<SettingsCard>
		<span slot="title" class={sequencerStatus?.isRunning ? "text-green-500" : "text-red-500"}>Installation {sequencerStatus?.isRunning ? "Running" : "Stopped"}</span>
		<GridForm>
			<SequencerProgressBar
				{sequencerStatus}
			/>
		</GridForm>
		<Button
			label="Start"
			onClick={startSequencer}
			disabled={sequencerStatus.isRunning}
		/>
		<Button
			label="Stop"
			onClick={stopSequencer}
		/>
	</SettingsCard>
{/if}