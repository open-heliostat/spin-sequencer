<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Text from './Text.svelte';
    import Slider from './Slider.svelte';
    import Button from './Button.svelte';
    import Checkbox from './Checkbox.svelte';
    import CANTopology from '~icons/tabler/topology-bus'
	import { get } from 'svelte/store';
	import Spinner from './Spinner.svelte';
    import type { CanSettings } from '$lib/types/models';

    export let restPath: string;
    
    let canSettings: CanSettings = {
        enabled: false,
        messagePack: false,
        txId: 0,
        rxId: 0,
        speed: 500,
        messageHistory: []
    };

    async function getCanSettings() {
        return getJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
            console.log("CAN Settings: ", canSettings);
        });
    }
    async function postCanSettings() {
        return postJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
        });
    }
    getCanSettings();

</script>

<SettingsCard>
	{#snippet icon()}
		<CANTopology class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span>Can Settings</span>
	{/snippet}
    {#await getCanSettings()}
    <Spinner></Spinner>
    {:then nothing}
    <GridForm>
        <Checkbox
            label="Enable"
            bind:value={canSettings.enabled}
            onChange={postCanSettings}
        />
        <Checkbox
            label="MessagePack"
            bind:value={canSettings.messagePack}
            onChange={postCanSettings}
        />
        <Slider
            label="Rx ID"
            min={1}
            max={255}
            step={1}
            bind:value={canSettings.rxId}
            onChange={postCanSettings}
        />
        <Slider
            label="Speed"
            min={1}
            max={1000}
            step={1}
            bind:value={canSettings.speed}
            onChange={postCanSettings}
        />
    </GridForm>
    {/await}
</SettingsCard>